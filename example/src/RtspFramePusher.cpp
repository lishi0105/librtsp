/*
 * @Author: 李石
 * @Date: 2024-08-07 17:02:50
 * @LastEditors: lishi
 * @LastEditTime: 2024-08-12 15:27:44
 * @Description: 
 * Copyright (c) 2024 by ${lishi0105@163.com}, All Rights Reserved. 
 */
#include <iostream>
#include <string.h>
#include <time.h>
#include "rtsp_comm.h"
#include "RtspFramePusher.hpp"

uint64_t RtspFramePusher::GetMillTimestamp() 
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)(ts.tv_sec) * 1000000 + ts.tv_nsec / 1000;
}

void RtspFramePusher::onClientConnect(char *clientIP,int clientPort, void *userData)
{
    if(clientIP == NULL)return;
    rtsp_info("rtsp client[%s:%d] login\n", clientIP, clientPort);
}

void RtspFramePusher::onClientDisconnect(char *clientIP, void *userData)
{
    if(clientIP == NULL)return;
    rtsp_info("rtsp client [%s] logout\n", clientIP);
}

RtspFramePusher::RtspFramePusher(const std::string &app, const std::string &stream, int codec_id, int port) :
    m_rtsp_handle(nullptr),
    m_app_name(app),
    m_stream_name(stream),
    m_output_url(""),
    m_codec_id(codec_id),
    m_out_port(port),
    m_init_success(false)
{
    if(m_codec_id != 0 && m_codec_id != 1){
        return;
    }

    rtsp_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.port = m_out_port;
    cfg.audioCodec = RTSP_CODEC_ID_NONE;
    cfg.videoCodec = m_codec_id == 0 ? RTSP_CODEC_ID_VIDEO_H264 : RTSP_CODEC_ID_VIDEO_H265;
    sprintf(cfg.suffix,"/%s/%s", m_app_name.c_str(), m_stream_name.c_str());
    m_rtsp_handle = rtsp_init(&cfg, &RtspFramePusher::onClientConnect, &RtspFramePusher::onClientDisconnect, NULL);
    if(m_rtsp_handle == nullptr){
        rtsp_err("rtsp init err\n");
        return;
    }
    m_output_url = std::string(m_rtsp_handle->outRtspUrl);
    m_init_success = true;
}

RtspFramePusher::~RtspFramePusher()
{
    if(m_rtsp_handle != nullptr){
        rtsp_uinit(m_rtsp_handle);
    }
}

bool RtspFramePusher::PushFrameData(const uint8_t *data, size_t size, uint64_t pts_)
{
    if(!m_init_success){
        rtsp_err("RtspFramePusher not init success\n");
        return false;
    }
    if(data == nullptr || size == 0){
        rtsp_err("data is nullptr or size:[%lu] is 0\n", size);
        return false;
    }
    uint64_t pts = pts_;
    if(pts == 0){
        pts = RtspFramePusher::GetMillTimestamp();
    }
    if(rtsp_server_write_video(m_rtsp_handle, data, size, pts) != RTSP_SUCC){
        rtsp_err("rtsp_server_write_video failed\n");
        return false;
    }
    return true;
}
