
LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_objdetect -lopencv_ml -lopencv_video -lopencv_flann -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_imgcodecs
PATH_INCLUDES = /opt/installation/OpenCV-3.4.4/include
PATH_LIB = /opt/installation/OpenCV-3.4.4/lib/

Task3: Lab1.3.1AVSA2020.o fgseg.o ShowManyImages.o
	g++ Lab1.3.1AVSA2020.o fgseg.o ShowManyImages.o -o Task3 -L$(PATH_LIB) $(LIBS) -lm

Lab1.3.1AVSA2020.o: Lab1.3.1AVSA2020.cpp fgseg.hpp ShowManyImages.hpp
	g++ -c Lab1.3.1AVSA2020.cpp -I$(PATH_INCLUDES) -O

fgseg.o: fgseg.cpp fgseg.hpp
	g++ -c fgseg.cpp -I$(PATH_INCLUDES) -O

ShowManyImages.o: ShowManyImages.cpp
	g++ -c ShowManyImages.cpp -I$(PATH_INCLUDES) -O

clean:
	rm *.o Task3
