LIBS_PATH = /lib/x86_64-linux-gnu/ /usr/lib/x86_64-linux-gnu/
INCS =
LIBS = -l pthread

SRC_LIBQUEUE = tpool.cpp
OBJ_LIBQUEUE = ${SRC_LIBQUEUE:.cpp=.o}

SRC_TEST = test_tpool.cpp
OBJ_TEST = ${SRC_TEST:.cpp=.o}

CC = g++
CFLAGS = -std=c++11 -c -g -Wall -Werror -pie -fPIC ${INCS}
LDFLAGS = ${LIBS}

all: libqueue++.so test_tpool

.cpp.o:
		@echo CC $<
		@${CC} ${CFLAGS} $<

libqueue++.so: ${OBJ_LIBQUEUE}
		@echo CC -o $@
		@${CC} -shared -o $@ ${OBJ_LIBQUEUE} ${LDFLAGS}

test_tpool: ${OBJ_TEST}
		@echo CC -o $@
		@${CC} -o $@ ${OBJ_TEST} ${LDFLAGS} -L $(CURDIR) -l queue++ -Wl,-rpath,$(CURDIR)

clean:
		@echo Cleaning
		@rm -f ${OBJ_LIBQUEUE} ${OBJ_TEST}
		@rm -f test_tpool

