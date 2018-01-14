# LIB=-L/usr/informix/lib/c++
# INC=-I/usr/informix/incl/c++ /opt/informix/incl/public
# INC=-I/usr/include
# LIB=-L /usr/lib
INC=-I/opencv-3.4.0/build_wasm
PRE=--preload-file ./shape_predictor_68_face_landmarks.dat
WASM=-s WASM=1

default:	main

main:   face-replace.cpp
	gcc $(INC) -c face-replace.cpp
# test.cpp
# gcc -Wall $(LIB) $(INC) -c test.cpp

wasm:	face-replace.cpp
	emcc $(INC) $(PRE) -std=c++11 $(WASM) -c face-replace.cpp

another: face-replace.cpp
	em++ $(PRE) -std=c++11 $(WASM) $(LIB) $(INC) -c face-replace.cpp

clean:
	rm -r test.o make.out
