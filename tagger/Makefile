CFLAGS=-g3 -O0 `mysql_config --cflags`
LIBS=/usr/lib/arm-linux-gnueabihf/libmysqlclient.a /usr/lib/arm-linux-gnueabihf/libgnutls.so -lpthread -ldl -lz

all:
	g++ ${CFLAGS} tagger.c -o mtagger ${LIBS}
