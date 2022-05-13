
LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_objdetect -lopencv_ml -lopencv_video -lopencv_flann -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_imgcodecs
PATH_INCLUDES = /opt/installation/OpenCV-3.4.4/include
PATH_LIB = /opt/installation/OpenCV-3.4.4/lib/

Task4.1: main.o utils.o 
	g++ main.o utils.o -o Task4.1 -L$(PATH_LIB) $(LIBS) -lm

main.o: main.cpp utils.cpp utils.hpp
	g++ -c main.cpp -I$(PATH_INCLUDES) -O

utils.o: utils.cpp utils.hpp
	g++ -c utils.cpp -I$(PATH_INCLUDES) -O

clean:
	rm *.o Task4.1
