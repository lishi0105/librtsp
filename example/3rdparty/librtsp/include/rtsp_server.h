/*
 * @Description: 
 * @Version: 
 * @Autor: 李石
 * @Date: 2021-11-20 20:49:02
 * @LastEditors: lishi
 * @LastEditTime: 2024-08-12 11:46:46
 */
#ifndef __RTSP_SERVER_H__
#define __RTSP_SERVER_H__
/*
 * a simple RTSP server demo
 * RTP over UDP/TCP H264/G711a 
*/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RTSP_SUCCESS     0
#define RTSP_FAILURE     (-1)
#define RTSP_SUCC        RTSP_SUCCESS
#define RTSP_FAIL        RTSP_FAILURE
typedef enum _rtsp_codec_id{
    RTSP_CODEC_ID_NONE = 0,
    RTSP_CODEC_ID_VIDEO_H264 = 0x0001,    /*codec_data is SPS + PPS frames*/
    RTSP_CODEC_ID_VIDEO_H265,            /*codec_data is VPS + SPS + PPS frames*/
    RTSP_CODEC_ID_VIDEO_MPEG4,            /*now not support*/
    RTSP_CODEC_ID_AUDIO_G711A = 0x4001,    /*codec_data is NULL*/
    RTSP_CODEC_ID_AUDIO_G711U,            /*codec_data is NULL*/
    RTSP_CODEC_ID_AUDIO_G726,            /*codec_data is bitrate (int)*/
    RTSP_CODEC_ID_AUDIO_AAC,            /*codec_data is audio specific config (2bytes). frame type is ADTS*/
} rtsp_codec_id;

typedef struct _rtsp_cfg
{
    int port;
    char suffix[32];
    rtsp_codec_id videoCodec;
    rtsp_codec_id audioCodec;
}rtsp_cfg;

typedef void *rtsp_demo_handle;
typedef void *rtsp_session_handle;
#define RTSP_CALLBACK 
typedef void (RTSP_CALLBACK* rtspClientConnect)(char *clientIP,int clientPort, void *user_data);
typedef void (RTSP_CALLBACK* rtspClientDisconnect)(char *clientIP, void *user_data);
typedef struct _rtsp_handle
{
    rtsp_demo_handle g_demo;
    rtsp_session_handle g_session;
    rtspClientConnect    pRtspClientConnect;
    rtspClientDisconnect pRtspClientDisconnect;
    void *pUserData;
    char outRtspUrl[240];
}RtspHandle;

RtspHandle* rtsp_init(const rtsp_cfg *cfg, rtspClientConnect onConnect,rtspClientDisconnect onDisconnect, void *userData);
int rtsp_server_write_video(const RtspHandle*, const uint8_t *frame, int len, uint64_t ts);
int rtsp_server_write_audio(const RtspHandle*, const uint8_t *frame, int len, uint64_t ts);
int rtsp_uinit(RtspHandle* );
void rtsp_codec_to_char(rtsp_codec_id codecID,char *codecBuf,int buflen);

#ifdef __cplusplus
}
#endif
#endif
