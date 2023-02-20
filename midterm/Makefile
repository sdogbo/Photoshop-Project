# Makefile

CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -g
LIBS += -lm

# Links together files needed to create project executable
project: project.o ppm_io.o image_manip.o
	$(CC) -o project project.o ppm_io.o image_manip.o $(LIBS)

# Links together files needed to create checkerboard executable
checkerboard: checkerboard.o ppm_io.o
	$(CC) -o checkerboard checkerboard.o ppm_io.o
	
# Links together files needed to create img_cmp executable
img_cmp: img_cmp.o ppm_io.o 
	$(CC) -o img_cmp img_cmp.o ppm_io.o $(LIBS)

# Compiles img_cmp.c to create img_cmp.o
img_cmp.o: img_cmp.c ppm_io.h
	$(CC) $(CFLAGS) -c img_cmp.c $(LIBS)

# Compiles checkerboard.c to create project.o
checkerboard.o: checkerboard.c ppm_io.h
	$(CC) -c checkerboard.c 

# Compiles project.c to create project.o
project.o: project.c ppm_io.h image_manip.h
	$(CC) $(CFLAGS) -c project.c $(LIBS)

# Compiles ppm_io.c to create ppm_io.o
ppm_io.o: ppm_io.c ppm_io.h
	$(CC) $(CFLAGS) -c ppm_io.c

# Compiles image_manip.c to create image_manip.o
image_manip.o: image_manip.c image_manip.h
	$(CC) $(CFLAGS) -c image_manip.c	

# Removes all object files and the executable named project
clean:
	rm -f *.o project
