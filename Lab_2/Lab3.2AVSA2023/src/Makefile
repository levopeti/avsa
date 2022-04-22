
LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_objdetect -lopencv_ml -lopencv_video -lopencv_flann -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_imgcodecs
PATH_INCLUDES = /opt/installation/OpenCV-3.4.4/include
PATH_LIB = /opt/installation/OpenCV-3.4.4/lib/

Task3.2: main31b.o kalman_filter.o ShowManyImages.o
	g++ main31b.o kalman_filter.o ShowManyImages.o -o Task3.2 -L$(PATH_LIB) $(LIBS) -lm

main31b.o: main31b.cpp kalman_filter.cpp kalman_filter.hpp ShowManyImages.cpp ShowManyImages.hpp
	g++ -c main31b.cpp -I$(PATH_INCLUDES) -O

kalman_filter.o: kalman_filter.cpp kalman_filter.hpp
	g++ -c kalman_filter.cpp -I$(PATH_INCLUDES) -O

ShowManyImages.o: ShowManyImages.cpp ShowManyImages.hpp
	g++ -c ShowManyImages.cpp -I$(PATH_INCLUDES) -O

clean:
	rm *.o Task3.2
