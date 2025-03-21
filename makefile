CC = gcc
CFLAGS = -Wall -g -MMD -MP -lpthread
TARGET = logistics-station-system

SRCS = main.c packages.c price.c shipping_order.c sqlite3.c database.c
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.c=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	powershell -Command "rm *.o, *.d, $(TARGET).exe -Force"

.PHONY: all clean
