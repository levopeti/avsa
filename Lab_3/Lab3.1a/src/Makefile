
LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_objdetect -lopencv_ml -lopencv_video -lopencv_flann -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_imgcodecs
PATH_INCLUDES = /opt/installation/OpenCV-3.4.4/include
PATH_LIB = /opt/installation/OpenCV-3.4.4/lib/

Task3: Lab2.0AVSA2020.o blobs.o ShowManyImages.o
	g++ Lab2.0AVSA2020.o blobs.o ShowManyImages.o -o Task3 -L$(PATH_LIB) $(LIBS) -lm

Lab2.0AVSA2020.o: Lab2.0AVSA2020.cpp blobs.hpp ShowManyImages.hpp
	g++ -c Lab2.0AVSA2020.cpp -I$(PATH_INCLUDES) -O

blobs.o: blobs.cpp blobs.hpp
	g++ -c blobs.cpp -I$(PATH_INCLUDES) -O

ShowManyImages.o: ShowManyImages.cpp
	g++ -c ShowManyImages.cpp -I$(PATH_INCLUDES) -O

clean:
	rm *.o Task3
