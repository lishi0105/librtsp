/*
 * @Author: 李石
 * @Date: 2024-08-07 15:35:50
 * @LastEditors: lishi
 * @LastEditTime: 2024-08-12 15:10:19
 * @Description: 
 * Copyright (c) 2024 by ${lishi0105@163.com}, All Rights Reserved. 
 */
/*************************************************************************
	> File Name: rtsp_comm.h
	> Author: bxq
	> Mail: 544177215@qq.com 
	> Created Time: Sunday, December 20, 2015 AM07:37:50 CST
 ************************************************************************/

#ifndef __LIBRTSP_COMM_DBG_H__
#define __LIBRTSP_COMM_DBG_H__

#include <stdio.h>

// #define _DEBUG

#ifdef _DEBUG
#define dbg(fmt, ...) do {printf("[DEBUG %s:%d:%s] " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);} while(0)
#define info(fmt, ...) do {printf("[INFO  %s:%d:%s] " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);} while(0)
#define warn(fmt, ...) do {printf("[WARN  %s:%d:%s] " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);} while(0)
#else
#define dbg(fmt, ...) 
#define info(fmt, ...)
#define warn(fmt, ...)
#endif
#define err(fmt, ...) do {printf("[ERROR %s:%d:%s] " fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);} while(0)

#ifdef __WIN32__
#define FD_SETSIZE 1024
//#include <windows.h>
#include <winsock2.h>
#define usleep(x) Sleep((x)/1000)
#define snprintf _snprintf
#endif

#if defined(__linux__) || defined(linux) || defined(__gnu_linux__)

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#ifndef __RTSP_FILENAME__
#define __RTSP_FILENAME__                                                         \
    (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 \
                                      : __FILE__)
#endif
#define RESET_COLOR  "\x1b[0m"
#define DEBUG_COLOR  "\x1b[36m"  // Cyan
#define INFO_COLOR   "\x1b[32m"  // Green
#define WARN_COLOR   "\x1b[33m"  // Yellow
#define ERROR_COLOR  "\x1b[31m"  // Red

#define GET_TIMESTAMP(__rtsp_dbg_time_buffer__) do { \
    struct timeval __rtsp_dbg_tv__; \
    size_t len = 0; \
    gettimeofday(&__rtsp_dbg_tv__, NULL); \
    struct tm *__rtsp_dbg_tm_info__ = localtime(&__rtsp_dbg_tv__.tv_sec); \
    len = strftime(__rtsp_dbg_time_buffer__, 30, "%Y-%m-%d %H:%M:%S", __rtsp_dbg_tm_info__); \
    snprintf(__rtsp_dbg_time_buffer__ + len, 10, ".%06ld", __rtsp_dbg_tv__.tv_usec); \
} while(0)

#define rtsp_dbg(fmt, ...) do { \
    char __rtsp_dbg_time_buffer__[30]; \
    GET_TIMESTAMP(__rtsp_dbg_time_buffer__); \
    printf(DEBUG_COLOR "[%s D %s:%d] " fmt RESET_COLOR, \
           __rtsp_dbg_time_buffer__, __RTSP_FILENAME__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define rtsp_info(fmt, ...) do { \
    char __rtsp_dbg_time_buffer__[30]; \
    GET_TIMESTAMP(__rtsp_dbg_time_buffer__); \
    printf(INFO_COLOR "[%s I %s:%d] " fmt RESET_COLOR, \
           __rtsp_dbg_time_buffer__, __RTSP_FILENAME__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define rtsp_warn(fmt, ...) do { \
    char __rtsp_dbg_time_buffer__[30]; \
    GET_TIMESTAMP(__rtsp_dbg_time_buffer__); \
    printf(WARN_COLOR "[%s W %s:%d] " fmt RESET_COLOR, \
           __rtsp_dbg_time_buffer__, __RTSP_FILENAME__, __LINE__, ##__VA_ARGS__); \
} while(0)

#define rtsp_err(fmt, ...) do { \
    char __rtsp_dbg_time_buffer__[30]; \
    GET_TIMESTAMP(__rtsp_dbg_time_buffer__); \
    printf(ERROR_COLOR "[%s E %s:%d] " fmt RESET_COLOR, \
           __rtsp_dbg_time_buffer__, __RTSP_FILENAME__, __LINE__, ##__VA_ARGS__); \
} while(0)

#endif // defined(__linux__) || defined(linux) || defined(__gnu_linux__)

#endif //__LIBRTSP_COMM_DBG_H__

