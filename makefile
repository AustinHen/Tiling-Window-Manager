CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lX11
TARGET = twm
SRCS = main.c mgr.c workSpaceUtils.c tilingLogic.c logicTests.c
OBJS = $(SRCS:.c=.o)
HEADERS = mgr.h

all: $(TARGET)

$(TARGET): $(HEADERS) $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
