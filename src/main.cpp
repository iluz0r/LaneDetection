#include "LineDetection.h"
#include "SignalDetect.h"

#define CLASSIFIER_FILE "FilesClassifier/cascadeNew.xml"

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
	 Mat3b bgr(Vec3b(62, 226, 251));
	 convertColorFromBGR2HSV(bgr);
*/
	VideoCapture cap;
	Mat capImg;
	vector<Vec4f> lines;

	// Detect variables initialization
	Classificatore clas = Classificatore(/* Leggere da file */);
	//Caricamento del classificatore ViolaJones
	CascadeClassifier trainCascade;
	if (!trainCascade.load(CLASSIFIER_FILE)) {
		cout << "Error loading classifier" << endl;
		return 1;
	}

	cap.set(CV_CAP_PROP_FPS, 10);
	cap.set(CV_CAP_PROP_BUFFERSIZE, 10);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 600);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 800);
	cap.open("video/linefollowing.mp4");

	if (!cap.isOpened()) {
		cout << "Errore" << endl;
		return 1;
	}
	while (cap.isOpened()) {
		if (cap.grab()) {
			cap.retrieve(capImg);
			/*
			Mat colorBalancedImg;
			LineDetection::colorBalancing(capImg, colorBalancedImg, 1);
			imshow("Color Balanced", colorBalancedImg);
			*/
			lines = LineDetection::detectLines(capImg);
			float leftWheel = 1, rightWheel = 1;
			LineDetection::calcAdjParams(lines, capImg, leftWheel, rightWheel);
			cout << "Left wheel:" << leftWheel << ", Right wheel: "
					<< rightWheel << endl;
			/*
			SignalDetect::detectAndClassifySignal(capImg, trainCascade, clas);
			imshow("result", capImg);
			waitKey(10);
			*/
		} else {
			cap.release();
		}
	}

}
