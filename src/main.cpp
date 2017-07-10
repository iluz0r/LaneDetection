#include "LineDetection.h"

#include <iostream>

int main(int argc, char **argv) {
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
			if (frame > 10) {
				LineDetection::detectLines(cap_img);
			}
		} else {
			cap.release();
		}
	}

}
