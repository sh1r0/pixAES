.PHONY: all en de run

PROG=encrypt
PROG2=decrypt

OPENCV = -I "C:\opencv\build\include" -L "C:\opencv\build\x86\mingw\lib" -lopencv_core245 -lopencv_highgui245 -lopencv_imgproc245

all: $(PROG) $(PROG2)

$(PROG): $(PROG).cpp AES.cpp
	g++ -o $@ $^ $(OPENCV)

$(PROG2): $(PROG2).cpp AES.cpp
	g++ -o $@ $^ $(OPENCV)

en:
	./$(PROG)

de:
	./$(PROG2)

run:
	./$(PROG)
	./$(PROG2)