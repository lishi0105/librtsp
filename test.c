#include "rtsp_demo.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PT_H264 0
#define PT_H265 1
int main()
{
	rtsp_cfg cfg;
	int i = 0;
    int	  enPayLoad[2]  = {PT_H264, PT_H264};
    int   rtspPort[2]   = {8553,8554};
    cfg.session_count = 2;
    char videoCodec[64]={0};
    char audioCodec[64]={0};
    for(i=0;i<2;i++){
        cfg.session_cfg[i].port = rtspPort[i];
        cfg.session_cfg[i].audioCodec = RTSP_CODEC_ID_NONE;
        if(enPayLoad[i]==PT_H264){
            cfg.session_cfg[i].videoCodec = RTSP_CODEC_ID_VIDEO_H264;
        }else if(enPayLoad[i]==PT_H265){
            cfg.session_cfg[i].videoCodec = RTSP_CODEC_ID_VIDEO_H265;
        }else{
            printf("enPayLoad[%d] : %derr\n",i,enPayLoad[i]);
        }
        memset(videoCodec,0,sizeof(videoCodec));
        memset(audioCodec,0,sizeof(audioCodec));
        rtsp_codec_to_char(cfg.session_cfg[i].videoCodec,videoCodec,sizeof(videoCodec));
        rtsp_codec_to_char(cfg.session_cfg[i].audioCodec,audioCodec,sizeof(audioCodec));
        memset(cfg.session_cfg[i].suffix,0,sizeof(cfg.session_cfg[i].suffix));
        sprintf(cfg.session_cfg[i].suffix,"/live/channel%d/video=%s/audio=%s",i,videoCodec,audioCodec);
    }
	while(1){
    	rtsp_init(&cfg);
		sleep(1);
		rtsp_uinit();
		printf("test ok\n");
	}
	return 0 ;
}