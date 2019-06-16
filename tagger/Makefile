CFLAGS=-g3 -O0 `mysql_config --cflags`
LIBS=/usr/lib/x86_64-linux-gnu/libmysqlclient.a -lpthread -ldl -lz

all:
	g++ ${CFLAGS} tagger.c -o tagger ${LIBS}
