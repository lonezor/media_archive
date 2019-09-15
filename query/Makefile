CFLAGS=-g3 -O0 `mysql_config --cflags` `pkg-config --cflags gtk+-3.0`
LIBS=/usr/lib/arm-linux-gnueabihf/libmysqlclient.a /usr/lib/arm-linux-gnueabihf/libgnutls.so -lpthread -ldl -lz `pkg-config --libs gtk+-3.0` -lcurl

all:
	g++ ${CFLAGS} main.cpp query.cpp object.cpp window.cpp -o mquery ${LIBS}
streamer:
	gcc streamer.c -o streamer `pkg-config --cflags --libs gstreamer-1.0`
