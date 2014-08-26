all:
	gcc -I/usr/include/opencv/ *.c -o laboratorio02 -lopencv_imgproc -lopencv_video -lopencv_highgui -lopencv_core -lm
