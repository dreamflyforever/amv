/*
   进行音频采集，采集pcm数据并直接保存pcm数据
   音频参数： 
   声道数：		1
   采样位数：	16bit、LE格式
   采样频率：	16000Hz
   运行示例:
   $ ./mono_capture hw:0 123.pcm
 */

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <signal.h>

#define AudioFormat SND_PCM_FORMAT_S16_LE  //指定音频的格式,其他常用格式：SND_PCM_FORMAT_U24_LE、SND_PCM_FORMAT_U32_LE
#define AUDIO_CHANNEL_SET   2 			  //1单声道   2立体声
#define AUDIO_RATE_SET 16000   //音频采样率,常用的采样频率: 44100Hz 、16000HZ、8000HZ、48000HZ、22050HZ

FILE *pcm_data_file=NULL;
int run_flag=0;
void exit_sighandler(int sig)
{
	run_flag=1;
}

int main(int argc, char *argv[])
{
	if(argc!=3)
	{
		printf("./capture hw:0 123.pcm \n");
		return 0;
	}
	int i;
	int err;
	char *buffer;
	int buffer_frames = 1024;
	unsigned int rate = AUDIO_RATE_SET;
	snd_pcm_t *capture_handle;// 一个指向PCM设备的句柄
	snd_pcm_hw_params_t *hw_params; //此结构包含有关硬件的信息，可用于指定PCM流的配置

	/*注册信号捕获退出接口*/
	signal(2,exit_sighandler);

	/*PCM的采样格式在pcm.h文件里有定义*/
	snd_pcm_format_t format=AudioFormat; // 采样位数：16bit、LE格式

	/*打开音频采集卡硬件，并判断硬件是否打开成功，若打开失败则打印出错误提示*/
	// SND_PCM_STREAM_PLAYBACK 输出流
	// SND_PCM_STREAM_CAPTURE  输入流
	if ((err = snd_pcm_open (&capture_handle, argv[1],SND_PCM_STREAM_CAPTURE,0))<0) 
	{
		printf("无法打开音频设备: %s (%s)\n",  argv[1],snd_strerror (err));
		exit(1);
	}
	printf("音频接口打开成功.\n");

	/*创建一个保存PCM数据的文件*/
	if((pcm_data_file = fopen(argv[2], "wb")) == NULL)
	{
		printf("无法创建%s音频文件.\n",argv[2]);
		exit(1);
	} 
	printf("用于录制的音频文件已打开.\n");

	/*分配硬件参数结构对象，并判断是否分配成功*/
	if((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) 
	{
		printf("无法分配硬件参数结构 (%s)\n",snd_strerror(err));
		exit(1);
	}
	printf("硬件参数结构已分配成功.\n");

	/*按照默认设置对硬件对象进行设置，并判断是否设置成功*/
	if((err=snd_pcm_hw_params_any(capture_handle,hw_params)) < 0) 
	{
		printf("无法初始化硬件参数结构 (%s)\n", snd_strerror(err));
		exit(1);
	}
	printf("硬件参数结构初始化成功.\n");

	/*
	   设置数据为交叉模式，并判断是否设置成功
	   interleaved/non interleaved:交叉/非交叉模式。
	   表示在多声道数据传输的过程中是采样交叉的模式还是非交叉的模式。
	   对多声道数据，如果采样交叉模式，使用一块buffer即可，其中各声道的数据交叉传输；
	   如果使用非交叉模式，需要为各声道分别分配一个buffer，各声道数据分别传输。
	 */
	if((err = snd_pcm_hw_params_set_access (capture_handle,hw_params,SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) 
	{
		printf("无法设置访问类型(%s)\n",snd_strerror(err));
		exit(1);
	}
	printf("访问类型设置成功.\n");

	/*设置数据编码格式，并判断是否设置成功*/
	if ((err=snd_pcm_hw_params_set_format(capture_handle, hw_params,format)) < 0) 
	{
		printf("无法设置格式 (%s)\n",snd_strerror(err));
		exit(1);
	}
	fprintf(stdout, "PCM数据格式设置成功.\n");

	/*设置采样频率，并判断是否设置成功*/
	if((err=snd_pcm_hw_params_set_rate_near(capture_handle,hw_params,&rate,0))<0) 
	{
		printf("无法设置采样率(%s)\n",snd_strerror(err));
		exit(1);
	}
	printf("采样率设置成功\n");

	/*设置声道，并判断是否设置成功*/
	if((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params,AUDIO_CHANNEL_SET)) < 0) 
	{
		printf("无法设置声道数(%s)\n",snd_strerror(err));
		exit(1);
	}
	printf("声道数设置成功.\n");

	/*将配置写入驱动程序中，并判断是否配置成功*/
	if ((err=snd_pcm_hw_params (capture_handle,hw_params))<0) 
	{
		printf("无法向驱动程序设置参数(%s)\n",snd_strerror(err));
		exit(1);
	}
	printf("参数设置成功.\n");

	/*使采集卡处于空闲状态*/
	snd_pcm_hw_params_free(hw_params);

	/*准备音频接口,并判断是否准备好*/
	if((err=snd_pcm_prepare(capture_handle))<0) 
	{
		printf("无法使用音频接口 (%s)\n",snd_strerror(err));
		exit(1);
	}
	printf("音频接口准备好.\n");

	/*配置一个数据缓冲区用来缓冲数据*/
	//snd_pcm_format_width(format) 获取样本格式对应的大小(单位是:bit)
	int frame_byte=snd_pcm_format_width(format)/8;
	buffer=malloc(buffer_frames*frame_byte*AUDIO_CHANNEL_SET); //2048
	printf("缓冲区分配成功.\n");

	/*开始采集音频pcm数据*/
	printf("开始采集数据...\n");
	while(1) 
	{
		/*从声卡设备读取一帧音频数据:2048字节*/
		if((err=snd_pcm_readi(capture_handle,buffer,buffer_frames))!=buffer_frames) 
		{
			printf("退出\n");
			exit(1);
		}
		/*写数据到文件: 音频的每帧数据样本大小是16位=2个字节*/
		fwrite(buffer,(buffer_frames*AUDIO_CHANNEL_SET),frame_byte,pcm_data_file);	
		if(run_flag)
		{
			printf("停止采集.\n");
			break;
		}
	}

	/*释放数据缓冲区*/
	free(buffer);

	/*关闭音频采集卡硬件*/
	snd_pcm_close(capture_handle);

	/*关闭文件流*/
	fclose(pcm_data_file);
	return 0;
}
