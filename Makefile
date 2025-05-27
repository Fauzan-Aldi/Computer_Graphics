CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lgdi32 -mwindows -municode
TARGET = box_plot

all: $(TARGET)

$(TARGET): box_plot.c 
	$(CC) $(CFLAGS) -o output/$(TARGET).exe box_plot.c $(LDFLAGS)

clean:
	