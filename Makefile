CROSS_COMPILE=aarch64-linux-gnu-
CC=$(CROSS_COMPILE)gcc
CXX=$(CROSS_COMPILE)g++
AR=$(CROSS_COMPILE)ar
RANLIB=$(CROSS_COMPILE)ranlib
STRIP=$(CROSS_COMPILE)strip

BUILD ?= Release

ifeq ($(BUILD),Debug)
    TARGET = librtspd.a
    CFLAGS += -g -Wall -D__LINUX__ -Wno-unused-function -DDEBUG
else
    TARGET = librtsp.a
    CFLAGS += -O2 -Wall -D__LINUX__ -Wno-unused-function
endif

DST_DIR = install
LIB_INC = rtsp_demo.h

all: $(TARGET)

$(TARGET): rtsp_server.o rtsp_msg.o rtp_enc.o stream_queue.o utils.o ourIPAddress.o
	$(AR) -r $(TARGET) *.o

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $^

install: all
	$(MAKE) -j32
	/bin/mkdir -p $(DST_DIR)/include
	cp $(TARGET) $(DST_DIR) -rfv
	cp *.h $(DST_DIR)/include -rfv

clean:
	/bin/rm -f *.o
	/bin/rm -f librtsp.a librtspd.a
	/bin/rm -rf $(DST_DIR)/
