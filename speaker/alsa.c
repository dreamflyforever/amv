#define ALSA_PCM_NEW_HW_PARAMS_API
 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alsa/asoundlib.h>
 
#define u32 unsigned int
#define u8   unsigned char
#define u16 unsigned short
 
 
snd_pcm_t *gp_handle;
snd_pcm_hw_params_t *gp_params;
snd_pcm_uframes_t g_frames;
char *gp_buffer;
u32 g_bufsize;
 
int set_hardware_params(int sample_rate, int channels, int format_size)
{
	int rc;
	/* Open PCM device for playback */    
	rc = snd_pcm_open(&gp_handle, "hw:0,0", SND_PCM_STREAM_PLAYBACK, 0);    
	if (rc < 0) {
		printf("unable to open pcm device\n"); 
		return -1;   
	} 
	
	/* Allocate a hardware parameters object */    
	snd_pcm_hw_params_alloca(&gp_params); 
	
	/* Fill it in with default values. */    
	rc = snd_pcm_hw_params_any(gp_handle, gp_params);
	if (rc < 0) {
		printf("unable to Fill it in with default values.\n");    
		goto err1;  
	}
 
	/* Interleaved mode */    
	rc = snd_pcm_hw_params_set_access(gp_handle, gp_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (rc < 0) {    
		printf("unable to Interleaved mode.\n");    
		goto err1;  
	}
		
	snd_pcm_format_t format;
	
	if (8 == format_size) {
		format = SND_PCM_FORMAT_U8;
	} else if (16 == format_size) {
		format = SND_PCM_FORMAT_S16_LE;
	} else if (24 == format_size) {
		format = SND_PCM_FORMAT_U24_LE;
	} else if (32 == format_size) {
		format = SND_PCM_FORMAT_U32_LE;
	} else {
		printf("SND_PCM_FORMAT_UNKNOWN.\n");    
		format = SND_PCM_FORMAT_UNKNOWN;
		goto err1; 
	}
    
	/* set format */    
	rc = snd_pcm_hw_params_set_format(gp_handle, gp_params, format);
	if (rc < 0) {
		printf("unable to set format.\n");    
		goto err1;  
	}
	
	/* set channels (stero) */    
	snd_pcm_hw_params_set_channels(gp_handle, gp_params, channels);
	if (rc < 0) {
		printf("unable to set channels (stero).\n");    
		goto err1;  
	}

	/* set sampling rate */       
	u32 dir;  
	rc = snd_pcm_hw_params_set_rate_near(gp_handle, gp_params, &sample_rate, &dir); 
	if (rc < 0) {    
		printf("unable to set sampling rate.\n");    
		goto err1;  
	} 
	
	/* Write the parameters to the dirver */    
	rc = snd_pcm_hw_params(gp_handle, gp_params);    
	if (rc < 0) {    
		printf("unable to set hw parameters: %s\n", snd_strerror(rc));    
		goto err1;  
	} 

	snd_pcm_hw_params_get_period_size(gp_params, &g_frames, &dir);
	g_bufsize = g_frames * 4;
	gp_buffer = (u8 *)malloc(g_bufsize);
	if (gp_buffer == NULL) {
		printf("malloc failed\n");
		goto err1;  
	}
 
	return 0;
	
err1:
	snd_pcm_close(gp_handle);
	return -1;	
}
 
 
int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("usage: %s filename.wav sample_rate channels "
		"format_size\n like ./player speaker.wav 16000 1 16\n", argv[0]);
		return -1;
	}
	
	FILE * fp = fopen(argv[1], "r");
	if (fp == NULL) {
		printf("can't open wav file\n");
		return -1;
	}
	int sample_rate = atoi(argv[2]);
	int channels = atoi(argv[3]);
	int format_size = atoi(argv[4]);
	int ret = set_hardware_params(sample_rate, channels, format_size);
	if (ret < 0) {
		printf("set_hardware_params error\n");
		return -1;
	}
	
	size_t rc;
	/*enable speaker*/
	system("echo 55 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio55/direction");	
	system("echo 1 > /sys/class/gpio/gpio55/value");
	while (1) {
		rc = fread(gp_buffer, g_bufsize, 1, fp);
		if (rc <1) {
			break;
		}
		
		ret = snd_pcm_writei(gp_handle, gp_buffer, g_frames);    
		if (ret == -EPIPE) {    
			printf("underrun occured\n");  
			break;  
		} else if (ret < 0) {    
			printf("error from writei: %s\n", snd_strerror(ret));    
			break;
		} 
	}

	snd_pcm_drain(gp_handle);    
	snd_pcm_close(gp_handle);    
	free(gp_buffer); 
	fclose(fp);
	system("echo 0 > /sys/class/gpio/gpio55/value");
	return 0;
}

