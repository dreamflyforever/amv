#ifndef __DEV_WAV_H__
#define __DEV_WAV_H__

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef struct wave_head {
    //riff chunk
    uint8_t RIFF_ID[4];
    uint32_t RIFF_size;
    uint8_t format[4];
    //fmt chunk
    uint8_t Fmt_ID[4];
    uint32_t fmt_size;
    uint16_t audio_format;
    uint16_t chanel_num;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bitspersample;
    //data chunk
    uint8_t Data_ID[4];
    uint32_t data_size;
}Wave_head;

int Wave_Parse_Head(char *wav_name, Wave_head* headinfo, FILE **fp);


#endif
