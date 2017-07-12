#include "SignalDetect.h"

using namespace cv;

SignalDetect::SignalDetect() {
	// TODO Auto-generated constructor stub

}

SignalDetect::~SignalDetect() {
	// TODO Auto-generated destructor stub
}
// Questa classe deve detectare il segnale e restituire la classe del segnale stesso
int SignalDetect::detectAndClassifySignal(Mat& input,
		CascadeClassifier & SignalCascade, Classificatore& clas) {

	Mat grayImg;

	cvtColor(input, grayImg, CV_BGR2GRAY);
	vector<Rect> faces;
	//Applicazione dell'algoritmo di Viola-Jones
	SignalCascade.detectMultiScale(grayImg, faces, SCALING_FACTOR,
	MIN_NEIGHBORS, 0, cvSize(SIGNAL_MIN_WIDTH, SIGNAL_MIN_HEIGHT),
			cvSize(SIGNAL_MAX_WIDTH, SIGNAL_MAX_HEIGHT));
	//Disegno dei bounding
	int max = 0, finalDecision = 0;
	vector<int> signaldecision(clas.getNumberOfClass(), 0);
	for (unsigned int i = 0; i < faces.size(); i++) {

		Point pt2(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
		Point pt1(faces[i].x, faces[i].y);

		Mat imgcap =
				input(
						Rect(faces[i].x, faces[i].y, faces[i].width,
								faces[i].height)).clone();
		rectangle(input, pt1, pt2, Scalar(0, 0, 255), 2, 8, 0);
		// Se trovo più segnali decido a maggiornanza dunque tengo traccia del massimo
		unsigned int decision = clas.getClassDecision(imgcap);
		signaldecision[decision]++;
		if (signaldecision[decision] > max) {
			max = signaldecision[decision];
			finalDecision = decision;
		}
		cout << "Ho deciso per " << clas.getClassName(decision) << endl;
	}

	return finalDecision;
}
