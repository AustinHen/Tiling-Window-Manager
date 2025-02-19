CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lX11
TARGET = twm
SRCS = src/main.c src/mgr.c src/workSpaceUtils.c src/tilingLogic.c src/keyHandle.c
OBJS = $(SRCS:.c=.o)
HEADERS = include/mgr.h

all: $(TARGET)

$(TARGET): $(HEADERS) $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
