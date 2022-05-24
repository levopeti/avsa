LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_objdetect -lopencv_ml -lopencv_video -lopencv_flann -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_imgcodecs
PATH_INCLUDES = /opt/installation/OpenCV-3.4.4/include
PATH_LIB = /opt/installation/OpenCV-3.4.4/lib/

test: test.o 
	g++ test.o -o test -L$(PATH_LIB) $(LIBS) -lm

test.o: test.cpp 
	g++ -c test.cpp -I$(PATH_INCLUDES) -O

clean:
	rm *.o test