/*
 * @Description: 
 * @Version: 
 * @Autor: 李石
 * @Date: 2023-10-16 11:57:04
 * @LastEditors: lishi
 * @LastEditTime: 2024-08-12 10:57:54
 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/resource.h>
#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <thread>
#include "rtsp_logger.h"
#include "RtspFramePusher.hpp"

using namespace RTSP::LOG;

static std::string gl_input_file = "";
static int32_t gl_input_codec = 0;

void printHexBuf(const uint8_t *p, int size)
{
    for(int i = 0; i < size; i++)
    {
        printf("%.2X ",p[i]);
    }
    printf("\n");
}

bool parseArgs(int argc, char *argv[])
{
    int opt;
    bool h_flag = false;
    bool i_flag = false;
    bool c_flag = false;
    while ((opt = getopt(argc, argv, "i:c:h")) != -1) {
        switch (opt) {
            case 'i':
                i_flag = true;
                gl_input_file = optarg;
                break;
            case 'c':
                c_flag = true;
                gl_input_codec = atoi(optarg);
                if(gl_input_codec != 0 && gl_input_codec != 1){
                    std::cerr << "Error: codec_id must be 0 or 1.\n";
                    return false;
                }
                break;
            case 'h':
                h_flag = true;
                break;
            default: /* '?' */
                std::cerr << "Usage: " << argv[0] << " -i <input url>\n";
                return -1;
        }
    }
    if (h_flag) {
        std::cout << "Help Information:"<<std::endl;
        std::cout << "-i <input url >  : Specify rtsp url"<<std::endl;
        std::cout << "-h               : Show help information."<<std::endl;
        return false;
    } 
    if(!i_flag) {
        std::cerr << "Error: Missing mandatory option '-i <input url>'.\n";
        std::cerr << "Usage: " << argv[0] << " -i <input url>\n";
        return false;
    }
    if(!c_flag) {
        std::cerr << "Error: Missing mandatory option '-c <input codec>'.\n";
        std::cerr << "Usage: " << argv[0] << " -i <input codec>\n";
        return false;
    }
    std::cout << "input : [" << gl_input_file << ":"<< gl_input_codec<<"]"<<std::endl;
    return true;
}

void onRtspClientConnect(char *ip, int port, void *user_data)
{
    if(ip == NULL)return;
    RTSP_LOG(TRACE)<<"rtsp client ["<<ip<<":"<<port<<"] login";
}

void onRtspClientDisconnect(char *ip, void *user_data)
{
    RTSP_LOG(TRACE)<<"rtsp client ["<<ip<<"] logout";
}

int main(int argc, char *argv[])
{
    if(!parseArgs(argc, argv)){
        gl_input_file = "save.264";
        gl_input_codec = 0;
    }
    const int rtspPortBase = 8553;
    FILE *fp = fopen(gl_input_file.c_str(), "rb");
    if(fp == NULL){
        RTSP_LOG(ERROR)<<"open video file ["<<gl_input_file<<"] error";
        return 0;
    }else{
        RTSP_LOG(INFO)<<"open video file ["<<gl_input_file<<"] success";
    }
    char hdr_file[270] = {0};
    sprintf(hdr_file, "%s.hdr", gl_input_file.c_str());
    FILE *fp_hdr = fopen(hdr_file, "rb");
    if(fp_hdr == NULL){
        RTSP_LOG(ERROR)<<"open hdr file ["<<hdr_file<<"] error";
        return 0;
    }else{
        RTSP_LOG(INFO)<<"open hdr file ["<<hdr_file<<"] success";
    }
    char hdr_buf[64] = {0};
    char frame_buf[1024*1024] = {0};
    auto pRtspPusher = RtspFramePusher::CreateShared("live", "proxyer", gl_input_codec, 8553);
    RTSP_LOG(INFO)<<pRtspPusher->output_url();
    if(pRtspPusher == nullptr){
        RTSP_LOG(ERROR)<<"rtsp init error";
        return 0;
    }
    while(fgets(hdr_buf, sizeof(hdr_buf), fp_hdr) != NULL){
        int frame_size = atoi(hdr_buf);
        if(frame_size <= 0 || frame_size > (int)(sizeof(frame_buf))){
            RTSP_LOG(ERROR)<<"frame size ["<<frame_size <<"] error";
            break;
        }
        if(fread(frame_buf, 1, frame_size, fp) != (size_t)frame_size){
            RTSP_LOG(ERROR)<<"read frame error";
            break;
        }
        auto pts = RtspFramePusher::GetMillTimestamp();
        RTSP_LOG(TRACE)<<"push frame ["<<frame_size<<"] ts: "<<pts;
        pRtspPusher->PushFrameData((uint8_t *)frame_buf, frame_size, pts);
        usleep(40*1000);
    }
    fclose(fp);fp = NULL;
    fclose(fp_hdr);fp_hdr = NULL;
    RTSP_LOG(INFO)<<"exit";
    return 0;
}