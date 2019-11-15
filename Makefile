LIBS_PATH = /lib/x86_64-linux-gnu/ /usr/lib/x86_64-linux-gnu/
INCS =
LIBS = -l pthread

SRC_QUEUE = tpool.cpp
OBJ_QUEUE = ${SRC_QUEUE:.cpp=.o}

CC = g++
CFLAGS = -std=c++11 -c -g -Wall -Werror -pie -fPIC ${INCS}
LDFLAGS = ${LIBS}

all: libqueue++.so

.cpp.o:
		@echo CC $<
		@${CC} ${CFLAGS} $<

libqueue++.so: ${OBJ_QUEUE}
		@echo CC -o $@
		@${CC} -shared -o $@ ${OBJ_QUEUE} ${LDFLAGS}

clean:
		@echo cleaning
		@rm -f ${OBJ_QUEUE}
		@rm -f test_tpool
