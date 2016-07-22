###################################################################
#
# colormaps 
#
# freeman.justin@gmail.com
#
##################################################################


OBJ=	./src/main.o \
	./src/kmeans.o \
	./src/colorfunctions.o \
	./src/compare.o \
	./src/interp.o \
	./src/jutil.o

# Compliler flags
INC=    -I./include/

CFLAGS=	-O3 -g -Wall

CC=	gcc $(CFLAGS) $(INC) 

# Libraries
LFLAGS=

# Executable

EXEC=	./bin/colormaps

$(EXEC):$(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(LFLAGS)

clean:
	rm $(OBJ)
	rm $(EXEC)
