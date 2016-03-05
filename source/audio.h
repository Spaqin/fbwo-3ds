#ifndef AUDIO_H
#define AUDIO_H

#define MUSIC_CHANNEL 1

#define BUFFER_SIZE 4096 //in samples

#define STACKSIZE (4 * 1024)

Thread threadHandle;
Handle threadRequest;

volatile bool runThread;
u8 last_half;
u16 last_buf_seq;
bool music_loaded;
Music music;

u8 audio_init(const char*);
void audio_music_load();
void audio_music_play();
void audio_music_pause();
void audio_music_check();
void audio_fini();
void looped_fread(u32 bytes_to_read, u32 start_point, u32 data_size, FILE* fp, u8* buffer); 
void looped_vorbis_read(OggVorbis_File *vf, u8* buffer, u32 length);

#endif //AUDIO_H
