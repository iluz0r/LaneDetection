#include "LineDetection.h"

#include <iostream>

// Mat3b bgr(Vec3b(100,24,90)) for example
void convertColorFromBGR2HSV(const Mat3b &bgr, Mat3b &hsv) {
	// Convert a BGR color to HSV
	cvtColor(bgr, hsv, COLOR_BGR2HSV);
	cout << hsv << endl;
}

int main(int argc, char **argv) {
	/*Mat3b hsv;
	 Mat3b bgr(Vec3b(0,0,0));
	 convertColorFromBGR2HSV(bgr, hsv);*/
	VideoCapture cap;
	Mat cap_img;

	int frame = 0;
	cap.set(CV_CAP_PROP_FPS, 10);
	cap.set(CV_CAP_PROP_BUFFERSIZE, 10);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 600);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 800);
	cap.open("video/video.h264");

	if (!cap.isOpened()) {
		cout << "Errore" << endl;
		return 1;
	}
	while (cap.isOpened()) {
		if (cap.grab()) {
			cap.retrieve(cap_img);
			frame++;
			if (frame > 300) {
				imshow("Origin", cap_img);
				LineDetection::detectLines(cap_img);
			}
		} else {
			cap.release();
		}
	}

}
