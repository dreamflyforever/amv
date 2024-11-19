  
板子使用与测试例子  
  
[板子与电脑连接调试](#1.板子使用)  
[搭建docker交叉编译环境](#2.编译环境)  
[测试用例](#3.测试例子)    
[板子烧录系统](#4.板子烧录系统)  
[常见问题](#5.常见问题)  
[下载代码](https://github.com/dreamflyforever/open_amv/releases/download/v1.0.0/open_amv.tar.gz) 
  
## 1.板子使用  
板子使用type c作为调试口，同时供电，请在电脑上下载adb，当板子通过tpye c与电脑连接后，执行命令adb shell就会进入到板子的linux系统命令行。  
  
## 2.编译环境  
2.1 搭建docker交叉编译环境。在宿主机器上下载doker，然后拉取镜像，执行以下命令  
docker pull luckfoxtech/luckfox_pico:1.0      #从 Docker Hub 下载镜像。  
sudo docker run -it --name open-amv -v /path/easy_test:/home luckfoxtech/luckfox_pico:1.0 /bin/bash  
2.2 如果需要第二次打开, 执行命令:  
sudo docker start -ai open-amv
2.3 添加工具为系统环境变量:vi ~/.bashrc 最后一行添加  
export PATH=/home/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin:$PATH  
(git pull https://gitee.com/LuckfoxTECH/luckfox-pico.git)
验证环境成功，在命令行输入arm-然后按下tab按键，是否看到下面输出，如有说明交叉编译环境装好  
arm-rockchip830-linux-uclibcgnueabihf-    
  
## 3.测试例子  
测试例子包含mipi csi摄像头camera，深度学习使用npu推理，喇叭speaker，gpio口控制ioctrl，图像处理opencv-mobile，摄像头工具调试v4l2-utils，录音程序mic，pwm控制。    

3.1 mipi csi 摄像头测试程序:  
cd camera; make 输出:   
arm-rockchip830-linux-uclibcgnueabihf-g++ -shared -o ./objs/libRTSPServer.so -pthread -Wl,--whole-archive ./objs/lib_rtsp_common.a ./objs/lib_os_common.a ./objs/lib_sock.a ./objs/lib_util.a ./objs/lib_rtspserver_common.a ./objs/lib_rtspserver_rtsp.a -Wl,--no-whole-archive  
在tests目录下执行make, 把rtspclient和libRTSPClient.so 推动到板子上，  
ls tests/  
Makefile              RTSPLiveStreamer.h  libRTSPServer.so  rtspclient.cpp  rtspserver.cpp  
RTSPLiveStreamer.cpp  libRTSPClient.so    rtspclient        rtspserver  
mkdir -p /oem/ws  
adb push rtspclient /oem/ws  
adb push libRTSPClient.so /oem/ws  
添加库的系统环境变量  
export LD_LIBRARY_PATH=./lib:/oem/ws:$LD_LIBRARY_PATH  
然后在板子上执行./rtspclient 就会保存video.264的视频文件。  
基本原理：系统默认自动跑起rtst server，只需要根据rtsp协议就可以获取摄像头数据。  

3.2 如果想用rndis协议，在usb连接端的linux上执行命令sudo ifconfig  enx7e17714ff78c 172.32.0.100 up, enx7e17714ff78c具体看你linux，可以通过执行命令ifconfig查看   
，然后在linux端修改rtspclient.cpp的地址为172.32.0.93, 编译生成rtspclient执行就可以保存视频了。  

注意：  
1. 确保安装好交叉编译工具链  
2. 程序自动保存视频文件video.264  
3. 需要自己创建文件夹mkdir -p /oem/ws  

3.2 深度学习使用npu推理，本测试例子使用yolov5s模型，模型输入大小是640 * 640，未经过量化，coco80的数据集。从摄像头直接获取数据输入模型，通过rknn推理保存推理结果的图片。  
执行命令build-linux.sh会生成目录  
npu/install/rv1106_linux_armhf/rknn__demo  
把该目录用adb push 到/oem/ws  
执行 RkLunch-stop.sh; ./rknn_yolov5_demo yolov5s-640-640.rknn 即可，如果检测到coco 80类数据集的东西会保存成图片。  
代码包含了使用opencv-mobile读取摄像头，解码和保存图片。  
如果看到日志出现“/dev/video11 does not support changing fps”说明摄像头没接触好，断电重新抽拔即可.

3.3 喇叭测试，使用命令cd speaker, 编译命令make生成player  
把目录下的alsa文件夹放到/oem/ws/share/alsa这里  
执行 ./player speaker.wav 44100 1 16 即可播放声音  
该程序依赖与libasound，使用alsa的API接口进行播放声音，其中libasound已经交叉编译好放入代码中了，请放心使用。  
  
3.4 麦克风测试，使用命令cd mic, 编译命令./compile.sh 生成capture程序录音，  
代码里面有参数可以修改采样率和分辨率。  
#define AudioFormat SND_PCM_FORMAT_S16_LE  //指定音频的格式,其他常用格式：SND_PCM_FORMAT_U24_LE、SND_PCM_FORMAT_U32_LE  
#define AUDIO_CHANNEL_SET   1  			  //1单声道   2立体声  
#define AUDIO_RATE_SET 44100    
电脑端执行:adb push capture /oem; 在板子上执行/ome/capture hw:0 123.pcm 即可

3.5 gpio口控制，进入ioctrl/pwm，执行命令  
mkdir build; cd build; cmake ..; make  
生成的程序可以控制io口拉高拉低电平和控制pwm输出。具体逻辑看代码。  
  
3.6 图像处理opencv-mobile测试，进入到cd opencv-mobile; mkdir build; cd build; cmake ..; 生成opencv-mobile-camera  
执行opencv-mobile-camera 可以从摄像头获取图片，然后进行处理拼接输出out.jpg。  
  
3.7 摄像头工具调试v4l2-utils该目录是已经交叉编译好的摄像头工具，可以方便用于查看摄像头的信息，比如支持的分辨率和格式帧率等。  
  
## 4.板子烧录系统  
找到对应版本直接执行命令./upgrade_tool uf update.img即可  
  
## 5.常见问题:  
5.1 docker安装：请先安装docker的服务器端,参考https://docs.docker.com/engine/install/  
5.2 用户磁盘不够, 使用df -h删除大文件，或使用/tmp 目录进行测试，注意重启后tmp文件里的东西会丢失。  
5.3. 摄像头的读取错误，执行RkLunch-stop.sh杀掉占用camera的程序。  
5.4 ioctrl和pwm的程序测试请使用1c6 和1c7 分别是54和55，pwm10，pwm11.  
