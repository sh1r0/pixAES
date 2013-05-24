.PHONY: all run clean

CC=g++
PROG=encrypt
PROG2=decrypt
PROG3=eraser
CFLAGS=
DEPENDENCY=AES.cpp md5.cpp
OPENCV_INC="C:\opencv\build\include" 
OPENCV_LIB="C:\opencv\build\x86\mingw\lib" 
OPENCV=-I $(OPENCV_INC) -L $(OPENCV_LIB) -lopencv_core245 -lopencv_highgui245 -lopencv_imgproc245
IMG=

all: $(PROG) $(PROG2) $(PROG3)

$(PROG): $(PROG).cpp $(DEPENDENCY)
	$(CC) -o $@ $^ $(CFLAGS) $(OPENCV)

$(PROG2): $(PROG2).cpp $(DEPENDENCY)
	$(CC) $(CFLAGS) -o $@ $^ $(OPENCV)

$(PROG3): $(PROG3).cpp
	$(CC) $(CFLAGS) -o $@ $^ $(OPENCV)

run: $(PROG) $(PROG2)
	./$(PROG) $(IMG)
	./$(PROG2)

clean:
	rm -f $(PROG) $(PROG2) *.o *.exe
