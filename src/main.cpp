#include "LineDetection.h"
#include "SignalDetect.h"

#define CLASSIFIER_FILE "FilesClassifier/cascade.xml"

#include <iostream>

// Mat3b bgr(Vec3b(100,24,90)) for example
void convertColorFromBGR2HSV(const Mat3b &bgr) {
	Mat3b hsv;
	// Convert a BGR color to HSV
	cvtColor(bgr, hsv, COLOR_BGR2HSV);
	cout << hsv << endl;
}

int main(int argc, char **argv) {
	/*
	 Mat3b bgr(Vec3b(102, 74, 227));
	 convertColorFromBGR2HSV(bgr);
	 */
	VideoCapture cap;
	Mat cap_img;
	float err = 0;
	int frame = 0;

	// Detect variables initialization
	Classificatore clas = Classificatore(/* Leggere da file */);
	//Caricamento del classificatore ViolaJones
	CascadeClassifier face_cascade;
	if (!face_cascade.load(CLASSIFIER_FILE)) {
		cout << "Error loading classifier" << endl;
		return 1;
	}

	cap.set(CV_CAP_PROP_FPS, 10);
	cap.set(CV_CAP_PROP_BUFFERSIZE, 10);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 600);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 800);

	cap.open("video/video.h264");
	//cap.open("/home/virus/Desktop/fifo264");
	//cap.open("/home/virus/Desktop/out.h264");

	if (!cap.isOpened()) {
		cout << "Errore" << endl;
		return 1;
	}
	while (cap.isOpened()) {
		if (cap.grab()) {
			cap.retrieve(cap_img);
			frame++;
			if (frame > 300) {
				//imshow("Origin", cap_img);
				//err = LineDetection::detectLines(cap_img);
				//cout << "Errore:" << err << endl;
				SignalDetect::detectAndClassifySignal(cap_img, face_cascade, clas);
			}
		} else {
			cap.release();
		}
	}

}
