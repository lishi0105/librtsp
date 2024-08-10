#ifndef _OUR_UPADDRESS_H_
#define _OUR_UPADDRESS_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

int rtsp_getOurIpAddr(char *ipaddr);
int rtsp_find_free_port(int port_base, uint32_t range);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif