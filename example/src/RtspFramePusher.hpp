/*
 * @Author: 李石
 * @Date: 2024-04-24 17:25:47
 * @LastEditors: lishi
 * @LastEditTime: 2024-08-12 10:36:53
 * @Description: 
 * Copyright (c) 2024 by ${lishi0105@163.com}, All Rights Reserved. 
 */
#ifndef _RTSP_FRAME_PUSHER_HPP_
#define _RTSP_FRAME_PUSHER_HPP_
#include <mutex>
#include <stdint.h>
#include <iostream>
#include <memory>
#include "rtsp_server.h"

class RtspFramePusher
{
public:
    using Ptr = std::shared_ptr<RtspFramePusher>;
    static Ptr CreateShared(const std::string &app, const std::string &stream, int codec_id, int port = 0)
    {
        return std::make_shared<RtspFramePusher>(app, stream, codec_id, port);
    }
    ~RtspFramePusher();
    RtspFramePusher(const std::string &app, const std::string &stream, int codec_id, int port = 0);
    bool PushFrameData(const uint8_t *data, size_t size, uint64_t pts = 0);
    bool PushSeiFrame(const uint8_t *data, size_t size);
    std::string output_url() const { return m_output_url; }
    static uint64_t GetMillTimestamp();
private:
    static void onClientConnect(char *clientIP,int clientPort, void *userData);
    static void onClientDisconnect(char *clientIP, void *userData);
private:
    RtspHandle *m_rtsp_handle;
    std::string m_app_name;
    std::string m_stream_name;
    std::string m_output_url;
    int32_t m_codec_id;
    int32_t m_out_port;
    bool m_init_success;
};
#endif // _RTSP_FRAME_PUSHER_HPP_