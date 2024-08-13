<!--
 * @Author: 李石
 * @Date: 2024-08-12 10:00:34
 * @LastEditors: lishi
 * @LastEditTime: 2024-08-13 08:23:32
 * @Description: 
 * Copyright (c) 2024 by ${lishi0105@163.com}, All Rights Reserved. 
-->
**原作者**
```bash
 Author: bxq
	> Mail: 544177215@qq.com 
	> Created Time: Sunday, December 20, 2015 AM07:37:50 CST
```
1. 对原作者ibrtsp的二次封装，便于直接使用;
2. 支持H264/H265编码的简单rtspserver库的;
3. 音频支持需要自行添加;
4. 经valgrind验证测试无内存问题，可放心食用;
5. 如有侵权请联系lishi0105@163.com，我将自行删除;

librtsp编译方式：
```bash
# Relase
make && make install
# Debug
make BUILD=Debug && make install
```

example编译方式
```bash
# Debug build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DCMAKE_TOOLCHAIN_FILE=../toolchain/aarch64-linux-gnu.cmake \
         -DCMAKE_INSTALL_PREFIX=../install
make -j$(nproc)
# Release build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_TOOLCHAIN_FILE=../toolchain/aarch64-linux-gnu.cmake \
         -DCMAKE_INSTALL_PREFIX=../install
make -j$(nproc)
```

example使用方式
1. 使用ffmpeg等工具解析264/265视频文件，将每帧长度按行保存到视频文件名.hdr文件中，例如save.264视频文件保存到save.264.hdr
2. 注意解析视频帧长度需将sps/pps/vps和I帧一起算长度；即配置帧不能单独送入librtsp；
3. 注意推送视频帧时时间戳需精确到微秒级，如果使用毫秒级时间戳VLC播放只能显示一帧，具体原因待查；
4. 运行方式：./librtsp_test -i 输入文件 -c 文件编码方式0:264 1:265 例如： ./librtsp_test -i save.264 -c 0