# make a default target which builds "writer" from "writer.c"
# and set up a variable CROSS_COMPILE which when supplied and when it is equal to aarch64-none-linux-gnu-
# a new compiler is used to make the target

CC = gcc
CROSS_COMPILER = aarch64-none-linux-gnu-gcc

all:
ifeq ($(CROSS_COMPILE), aarch64-none-linux-gnu-)
	$(CROSS_COMPILER) -o writer writer.c
	echo $(CROSS_COMPILER) is used
else
	$(CC) -o writer writer.c
	echo $(CC) is used 
endif

clean:
	rm -f writer *.o
