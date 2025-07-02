CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lX11 -lm
TARGET = twm
SRCS = src/main.c src/mgr.c src/workSpaceUtils.c src/tilingLogic.c src/keyHandle.c src/focus.c 
OBJS = $(SRCS:.c=.o)
HEADERS = include/mgr.h include/stb_image.h

all: $(TARGET)

$(TARGET): $(HEADERS) $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
