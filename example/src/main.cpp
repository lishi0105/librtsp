/*
 * @Description: 
 * @Version: 
 * @Autor: 李石
 * @Date: 2023-10-16 11:57:04
 * @LastEditors: lishi
 * @LastEditTime: 2024-08-12 15:29:59
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
#include "rtsp_comm.h"
#include "RtspFramePusher.hpp"

static std::string gl_input_file = "";
static int32_t gl_input_codec = 0;

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

int main(int argc, char *argv[])
{
    if(!parseArgs(argc, argv)){
        gl_input_file = "save.264";
        gl_input_codec = 0;
    }
    const int rtspPortBase = 8553;
    FILE *fp = fopen(gl_input_file.c_str(), "rb");
    if(fp == NULL){
        rtsp_err("open video file [%s] error\n", gl_input_file.c_str());
        return 0;
    }else{
        rtsp_info("open video file [%s] success\n", gl_input_file.c_str());
    }
    char hdr_file[270] = {0};
    sprintf(hdr_file, "%s.hdr", gl_input_file.c_str());
    FILE *fp_hdr = fopen(hdr_file, "rb");
    if(fp_hdr == NULL){
        rtsp_err("open hdr file [%s] error\n", hdr_file);
        return 0;
    }else{
        rtsp_info("open hdr file [%s] success\n", hdr_file);
    }
    char hdr_buf[64] = {0};
    char frame_buf[1024*1024] = {0};
    auto pRtspPusher = RtspFramePusher::CreateShared("live", "proxyer", gl_input_codec, 8553);
    rtsp_info("%s\n", pRtspPusher->output_url().c_str());
    if(pRtspPusher == nullptr){
        rtsp_err("rtsp init error\n");
        return 0;
    }
    while(fgets(hdr_buf, sizeof(hdr_buf), fp_hdr) != NULL){
        int frame_size = atoi(hdr_buf);
        if(frame_size <= 0 || frame_size > (int)(sizeof(frame_buf))){
            rtsp_err("frame size [%d] error\n", frame_size);
            break;
        }
        if(fread(frame_buf, 1, frame_size, fp) != (size_t)frame_size){
            rtsp_err("fread frame error\n");
            break;
        }
        auto pts = RtspFramePusher::GetMillTimestamp();
        rtsp_dbg("push frame: [%d] ts: [%lu]\n", frame_size, pts);
        pRtspPusher->PushFrameData((uint8_t *)frame_buf, frame_size, pts);
        usleep(40*1000);
    }
    fclose(fp);fp = NULL;
    fclose(fp_hdr);fp_hdr = NULL;
    rtsp_warn("exit\n");
    return 0;
}