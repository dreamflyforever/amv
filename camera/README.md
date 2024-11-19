### 摄像头测试程序
系统已经自动跑起rtst server，所以只需要跑起来提供客户端即可。

### 使用方式
确保系统已经安装好交叉编译工具链arm-rockchip830-linux-uclibcgnueabihf-,
主目录执行make命令, cd tests, 再make一次，生成可以执行的文件rtspclient和libRTSPClient.so动态库，把这
两个文件推到/oem/ws目录下，执行以下命令把库路径添加到系统既可
export LD_LIBRARY_PATH=./lib:/oem/ws:$LD_LIBRARY_PATH
然后执行./rtspclient

### 注意
1. 确保安装好交叉编译工具链
2. 程序自动保存视频文件video.264
3. 需要自己创建文件夹mkdir -p /oem/ws

### license
MIT by Jim
