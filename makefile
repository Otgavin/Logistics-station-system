CC = gcc
CFLAGS = -Wall -Werror -pedantic -g 
TARGET = logistics-station-system

SRCS = main.c packages.c price.c shipping_order.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	powershell -Command "rm *.o, $(TARGET).exe -Force"

.PHONY: all clean
