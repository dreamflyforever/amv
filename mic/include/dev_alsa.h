#ifndef __DEV_ALSA_H__
#define __DEV_ALSA_H__

#include "alsa/asoundlib.h"
#include <stdio.h>

typedef struct pcm_params {
    //pcm name like plughw:0,0
    char pcm_card_name[64];
    //pcm device handle
    snd_pcm_t *pcm_handle;
    //pcm stream direction
    snd_pcm_stream_t stream_direction;
    //specify configution for pcm
    snd_pcm_hw_params_t *hwparams;
    //channel num
    int channel_num;
    //周期数
    int periods;
    //sample format bit per sample
    snd_pcm_format_t sample_format;
    //PCM access type 交错模式或者交错
    snd_pcm_access_t access_type;
    //采样率
    int sample_rate;
    //每个周期的大小
    int period_szie;
    //frames 帧数
    int frames;

}Pcm_params;

int Alsa_Dev_init(Pcm_params *pcm);
int Alsa_Playback(Pcm_params *pcm, char **buf);
int Pcm_CloseDevice(Pcm_params *pcm);
int Alsa_SetVolum(int mode, int ctr_value);

#endif