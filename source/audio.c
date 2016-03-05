#define MUSIC_CHANNEL 1

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "include/vorbisfile.h"

#include "structs.h"
#include "audio.h"

u8 audio_init(const char* template)
{
    ndspInit();
	
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ndspSetOutputCount(1); // Num of buffers

	music_loaded = false;

    char buffer[80];
    sprintf(buffer, template, "music.ogg");

	music.file = malloc(sizeof(OggVorbis_File));

    int result = ov_fopen(buffer, music.file);
    if (result < 0)
    {
		printf("failed to open music.ogg\n");
		return 1;
    }

	vorbis_info* vi = ov_info(music.file, -1);

	music.sample_rate = vi->rate;

							//samples/bytes per sample/stereo
	music.total_buffer_size = BUFFER_SIZE * 2 * 2;
	
	music.first_data = linearAlloc(music.total_buffer_size);	
	music.second_data = linearAlloc(music.total_buffer_size);

	if(!music.first_data || !music.second_data)
	{
		printf("error allocating music memory\n");
		return 1;
	}


	music.chnl = MUSIC_CHANNEL;
	music.ndsp_format = vi->channels == 1 ? NDSP_FORMAT_MONO_PCM16 : NDSP_FORMAT_STEREO_PCM16;
	music.bytes_per_sample = 2;



	music.last_check = 0;
	runThread = true;
	svcCreateEvent(&threadRequest,0);
	aptOpenSession(); //make the ogg input/decoder thread run on second core
		printf("%d\n", APT_SetAppCpuTimeLimit(30));
	aptCloseSession();
	threadHandle = threadCreate(audio_music_load, 0, STACKSIZE, 0x3f, -1, true);
	return 0;
}

void audio_music_load()
{
	//first time init


	music.first = calloc(1, sizeof(ndspWaveBuf));

	music.second = calloc(1, sizeof(ndspWaveBuf));


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
	

	looped_vorbis_read(music.file, music.first_data, music.total_buffer_size);
	looped_vorbis_read(music.file, music.second_data, music.total_buffer_size);
	ndspChnSetPaused(music.chnl, true);
	ndspChnWaveBufAdd(music.chnl, music.first);
	ndspChnWaveBufAdd(music.chnl, music.second);
	music_loaded = true;

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
	
		looped_vorbis_read(music.file, music.second_data, music.total_buffer_size); //load the actual data

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
	if(music_loaded)
		ov_clear(music.file);
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

//legacy, not used anymore, but left so I can use it later if necessary
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

void looped_vorbis_read(OggVorbis_File *vf, u8* buffer, u32 length)
{
	int current_section;
	u32 bytes_read = 0;
	u32 total_read = 0;
	while(total_read != length)
	{
		bytes_read = ov_read(vf, (char*)buffer + total_read, length-total_read, 0, 2, 1, &current_section);
		total_read += bytes_read; 
		if(!bytes_read)
		{
			ov_raw_seek(vf, 0);
		}
	}

}

