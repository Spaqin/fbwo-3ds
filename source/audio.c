#define MUSIC_CHANNEL 1

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "structs.h"
#include "audio.h"

u8 audio_init(const char* template)
{
    ndspInit();
	
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ndspSetOutputCount(1); // Num of buffers

	music_loaded = false;

    char buffer[80];
    sprintf(buffer, template, "music.wav");

    music.file = fopen(buffer, "rb");
    if (!music.file)
    {
		printf("failed to open music.wav\n");
		return 1;
    }

    char signature[4];
	
    fread(signature, 1, 4, music.file);
	
    if( signature[0] != 'R' &&
		signature[1] != 'I' &&
		signature[2] != 'F' &&
		signature[3] != 'F')
    {
		printf("Wrong file format.\n");
		fclose(music.file);
		return 1;
    }

	fseek(music.file, 40, SEEK_SET);
	fread(&music.data_size, 4, 1, music.file);
	fseek(music.file, 22, SEEK_SET);
	fread(&music.channels, 2, 1, music.file);
	fseek(music.file, 24, SEEK_SET);
	fread(&music.sample_rate, 4, 1, music.file);
	fseek(music.file, 34, SEEK_SET);
	fread(&music.bits_per_sample, 2, 1, music.file);

	if(music.data_size == 0 || (music.channels != 1 && music.channels != 2) ||
		(music.bits_per_sample != 8 && music.bits_per_sample != 16))
	{
		printf("Corrupted wav file.\n");
		fclose(music.file);
		return 1;
	}

	music.chnl = MUSIC_CHANNEL;

	
	if(music.bits_per_sample == 8)
	{
		music.ndsp_format = (music.channels == 1) ?
			NDSP_FORMAT_MONO_PCM8 :
			NDSP_FORMAT_STEREO_PCM8;
	}
	else
	{
		music.ndsp_format = (music.channels == 1) ?
			NDSP_FORMAT_MONO_PCM16 :
			NDSP_FORMAT_STEREO_PCM16;
	}
	fseek(music.file, 44, SEEK_SET); //set the file pointer to the beginning of sound data

	music.bytes_per_sample = music.bits_per_sample >> 3;
	music.total_buffer_size = BUFFER_SIZE * music.channels * music.bytes_per_sample;

	music.first_data = linearAlloc(music.total_buffer_size);
	if(!music.first_data)
		return 1;
	music.second_data = linearAlloc(music.total_buffer_size);
	if(!music.second_data)
		return 1;

	music.first = calloc(1, sizeof(ndspWaveBuf));
	if(!music.first)
		return 1;
	music.second = calloc(1, sizeof(ndspWaveBuf));
	if(!music.second)
		return 1;

	music.first->data_vaddr = music.first_data;
	music.first->nsamples = BUFFER_SIZE;
	music.first->looping = false;
	music.first->status = NDSP_WBUF_FREE;

	music.second->data_vaddr = music.second_data;
	music.second->nsamples = BUFFER_SIZE;
	music.second->looping = false;
	music.second->status = NDSP_WBUF_FREE;

	ndspChnReset(MUSIC_CHANNEL);
	ndspChnSetInterp(MUSIC_CHANNEL, NDSP_INTERP_NONE);
	ndspChnSetRate(MUSIC_CHANNEL, music.sample_rate);
	ndspChnSetFormat(MUSIC_CHANNEL, music.ndsp_format);
	
	//load first data
	looped_fread(music.total_buffer_size, 44, music.data_size, music.file, music.first_data);
	looped_fread(music.total_buffer_size, 44, music.data_size, music.file, music.second_data);

	music.last_check = 0;
	runThread = true;
	svcCreateEvent(&threadRequest,0);
	threadHandle = threadCreate(audio_music_load, 0, STACKSIZE, 0x3f, -2, true);
	
	music_loaded = true;
	return 0;
}

void audio_music_load()
{

	while(runThread)
	{
		svcWaitSynchronization(threadRequest, U64_MAX);
		svcClearEvent(threadRequest);
		//past this point - means we need to swap the buffers, and the first is finished/used
		ndspWaveBuf* temp = music.first;
		u8* temp_data = music.first_data;

		music.first = music.second; //swap
		music.second = temp;

		music.first_data = music.second_data;
		music.second_data = temp_data; //swap data pointers
		//now the "first" is playing, and the second is not yet added		

		memset(music.second, 0, sizeof(ndspWaveBuf)); //reset the buffer struct
		music.second->data_vaddr = music.second_data;
		music.second->nsamples = BUFFER_SIZE;
		music.second->looping = false;
		music.second->status = NDSP_WBUF_FREE;
	
		looped_fread(music.total_buffer_size, 44, music.data_size, music.file, music.second_data); //load the actual data

		DSP_FlushDataCache(music.second_data, music.total_buffer_size);
		ndspChnWaveBufAdd(music.chnl, music.second);

	}
}
void audio_music_play()
{	
	if(!music_loaded)
		return;
	if(!ndspChnIsPlaying(music.chnl)) //first time init
	{
		ndspChnWaveBufAdd(music.chnl, music.first);
		ndspChnWaveBufAdd(music.chnl, music.second);
		music.last_check = 0;
		ndspChnSetPaused(music.chnl, false);
	}
	else
	{
		ndspChnSetPaused(music.chnl, false);
	}
}
void audio_music_pause()
{
	if(!music_loaded)
		return;
	if(!ndspChnIsPaused(music.chnl) && music_loaded)
	{
		ndspChnSetPaused(music.chnl, true);
	}
}

void audio_fini()
{
	ndspChnWaveBufClear(music.chnl);
	runThread = false;
	linearFree(music.first_data);
	linearFree(music.second_data);
	free(music.first);
	free(music.second);
	ndspExit();
}

void audio_music_check()
{
	if(!music_loaded)
		return;
	u32 cur_sample = ndspChnGetSamplePos(music.chnl);
	if(music.last_check > cur_sample && !ndspChnIsPaused(music.chnl)) //means it went to the second buffer - actually a bad idea for small buffer sizes
	{
		svcSignalEvent(threadRequest);
	}
	music.last_check = cur_sample;
	
}

void looped_fread(u32 bytes_to_read, u32 start_point, u32 data_size, FILE* fp, u8* buffer)
{
	u32 current_position = ftell(fp);
	while(bytes_to_read)
	{
		if(current_position + bytes_to_read <= start_point + data_size)
		{
			fread(buffer, 1, bytes_to_read, fp);
			return;
		}
		else //bytes_to_end < bytes_to_read
		{
			u32 bytes_to_end = start_point + data_size - current_position;
			fread(buffer, 1, bytes_to_end, fp);
			bytes_to_read -= bytes_to_end;
			buffer += bytes_to_end;
			fseek(fp, start_point, SEEK_SET); //set it to beginning
		}
	}

}

