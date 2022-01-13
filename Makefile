
CROSS_COMPILE=mips-linux-uclibc-gnu-
CC=$(CROSS_COMPILE)gcc
CXX=$(CROSS_COMPILE)g++
AR=$(CROSS_COMPILE)ar
RANLIB= $(CROSS_COOMPILE)ranlib
STRIP=$(CROSS_COMPILE)strip

TARGET = librtsp.a
DST_DIR = /home/user/workspace/T40_venc/3rdparty/librtsp/lib/
LIB_INC = rtsp_demo.h
CC ?= $(CROSS_COMPILE)gcc
CFLAGS += -g -Wall -D__LINUX__
#CFLAGS += -g -Wall 

all : rtsp_server.o rtsp_msg.o rtp_enc.o stream_queue.o utils.o ourIPAddress.o

	$(AR) -r $(TARGET) *.o
#	cp $(TARGET) $(DST_DIR) -rfv

%.o: %.c
    ##$(CC) -c $(CFLAGS) -o $@ $<
	$(CC) -c $(CFLAGS) -o $@ $^

clean :
	/bin/rm -f *.o
	/bin/rm -f $(TARGET)
