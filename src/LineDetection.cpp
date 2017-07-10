/*
 * LineDection.cpp
 *
 *  Created on: 10 lug 2017
 *      Author: angelo
 */

#include "LineDetection.h"

LineDetection::LineDetection() {

}

LineDetection::~LineDetection() {
// TODO Auto-generated destructor stub
}

float LineDetection::detectLines(const Mat &input) {
	Mat canny, cdst, hsv_img, gray_img, dest_HSV, dest_HSV_dilated, element,
			mask, hsv_mask, inputMod;
	inputMod = input(Rect(0, input.rows / 2, input.cols, input.rows / 2 - 1)).clone();

	assert(inputMod.type() == CV_8UC3);
	//Conversione HSV
	cvtColor(inputMod, hsv_img, CV_BGR2HSV);
	hsv_mask = hsv_img.clone();
	Mat tmp, yellow_mask, white_mask, red_mask, red_mask1;

	// Black MASK
	inRange(hsv_mask, Scalar(0, 0, 0), Scalar(180, 255, 60), tmp);
	int dilat_size = 30;
	int erode_size = 20;

	element = getStructuringElement(MORPH_RECT,
			Size(2 * dilat_size + 1, 2 * dilat_size + 1),
			Point(dilat_size, dilat_size));
	dilate(tmp, mask, element);
	element = getStructuringElement(MORPH_RECT,
			Size(2 * erode_size + 1, 2 * erode_size + 1),
			Point(erode_size, erode_size));

	erode(mask, mask, element);

	dilat_size = 5;

	element = getStructuringElement(MORPH_RECT,
			Size(2 * dilat_size + 1, 2 * dilat_size + 1),
			Point(dilat_size, dilat_size));

	imshow("MASK", mask);

	// Yellow threshold
	//inRange(hsv_mask, Scalar(20, 100, 100), Scalar(40, 255, 255), tmp);
	inRange(hsv_mask, Scalar(25, 140, 100), Scalar(45, 255, 255), tmp);
	bitwise_and(inputMod, inputMod, yellow_mask, tmp);
	dilate(yellow_mask, yellow_mask, element);

	imshow("MASK YELLOW", yellow_mask);

	// White threshold
	//inRange(hsv_mask, Scalar(-180, 0, 245), Scalar(180, 20, 255), tmp);
	//inRange(hsv_mask, Scalar(0, 0, 150), Scalar(180, 60, 255), tmp);
	inRange(hsv_mask, Scalar(0, 0, 235), Scalar(255, 5, 255), tmp);
	bitwise_and(inputMod, inputMod, white_mask, tmp);
	dilate(white_mask, white_mask, element);

	imshow("MASK WHITE", white_mask);

	// Red threshold
	//inRange(hsv_mask, Scalar(-5, 100, 100), Scalar(5, 255, 255), tmp);

	inRange(hsv_mask, Scalar(0, 140, 100), Scalar(15, 255, 255), tmp);
	//inRange(hsv_mask, Scalar(165, 140, 100), Scalar(180, 255, 255), red_mask1);
	//bitwise_or(tmp, red_mask1, red_mask, noArray());
	//namedWindow("red", WINDOW_AUTOSIZE);
	//imshow("red", red_mask);
	bitwise_and(inputMod, inputMod, red_mask, tmp);
	dilate(red_mask, red_mask, element);

	imshow("RED MASK", red_mask);

	// Final MASK
	bitwise_or(yellow_mask, white_mask, mask, noArray());
	bitwise_or(mask, red_mask, mask, noArray());

	// Applico Canny sull'immagine di partenza
	cvtColor(inputMod, gray_img, CV_BGR2GRAY);
	Canny(gray_img, canny, 10, 100, 3);

	cvtColor(canny, canny, CV_GRAY2RGB);

	vector<Vec4i> lines;

	Mat mask_output, result = gray_img.clone();
	cvtColor(yellow_mask, yellow_mask, CV_BGR2GRAY);
	// Faccio AND tra canny e la maschera del giallo
	bitwise_and(canny, canny, mask_output, yellow_mask);

	// Converto la Mat mask_output da 16 bits a 8 bits e a single-channel
	mask_output.convertTo(mask_output, CV_8U);
	cvtColor(mask_output, mask_output, CV_BGR2GRAY);

	imshow("mask_output contorni giallo ", mask_output);

	Vec4f l = LineDetection::calcStartEndYellowLine(mask_output);
	line(result, Point(200 * l[0] + l[2], 200 * l[1] + l[3]), Point(l[2], l[3]),
			Scalar(255, 100, 255), 3, 4);
	imshow("prova", result);
	waitKey(-1);

	//line(result, Point(l[0].x, l[0].y), Point(l[1].x, l[1].y), Scalar(255, 255, 0), 3,	4);
	LineDetection::detectAndShowLines(mask_output, result, Scalar(0, 255, 255));
	cvtColor(white_mask, white_mask, CV_BGR2GRAY);
	bitwise_and(canny, canny, mask_output, white_mask);
	LineDetection::detectAndShowLines(mask_output, result, Scalar(0, 0, 0));
	Vec4f lw = LineDetection::calcStartEndWhiteLine(mask_output, l);
	line(result, Point(200 * lw[0] + lw[2], 200 * lw[1] + lw[3]),
			Point(lw[2], lw[3]), Scalar(255, 100, 255), 3, 4);
	cvtColor(red_mask, red_mask, CV_BGR2GRAY);
	bitwise_and(canny, canny, mask_output, red_mask);
	LineDetection::detectAndShowLines(mask_output, result, Scalar(0, 0, 255));

	//	namedWindow("AND", WINDOW_AUTOSIZE);
	//	imshow("AND", mask_output);

	// Scelgo un punto P che per me è il punto medio della mia visuale
	// mi calcolo la distanza tra P e le due rette, rettifico la mia traiettoria confrontando quest'ultime
	// vado piu a dx o piu a sx se una distanza e piu grande dell'altra e viceversa
	Point P(inputMod.cols / 2, inputMod.rows / 2);

	float distWhite, distYellow;

	Point O(lw[2], lw[3]), O1(l[2], l[3]);
	float mw = lw[1] / lw[0], my = l[1] / l[0];

	distWhite = abs(P.x - (P.y - (O.y - mw * O.x)) / mw);
	distYellow = abs(P.x - (P.y - (O1.y - my * O1.x)) / my);

	circle(result, P, 5, Scalar(0, 0, 255), 8, 1, 0);
	circle(result, Point(P.x + distWhite / 2, P.y), 5, Scalar(255, 0, 0), 8, 1,
			0);
	circle(result, Point(P.x + distYellow / 2, P.y), 5, Scalar(0, 255, 0), 8, 1,
			0);
	float threshold = 10;

	imshow("LINE", result);

	if (distWhite - distYellow > threshold) {
		// correggo verso sinistra
		return 1 * distWhite / distYellow;
	}
	if (distWhite - distYellow < -threshold) {
		//correggo verso destra
		return -1 * distYellow / distWhite;
	}
	return 0;
}

void LineDetection::detectAndShowLines(const Mat &input, Mat &output,
		Scalar color) {
	vector<Vec4i> lines;
	HoughLinesP(input, lines, 1, CV_PI / 180 / 2, 40, 10, 10);
	//	Vec4i init, end;
	//	init=lines[0];
	//	end=lines[lines.size()-1];
	//	Vec4i line=Vec4i(init[0],init[1],end[2],end[3]);

	for (size_t i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		line(output, Point(l[0], l[1]), Point(l[2], l[3]), color, 3, 4);
	}

}

Vec4f LineDetection::calcStartEndYellowLine(const Mat &input) {
	vector<Vec4i> lines;
	Vec4f yline;

	HoughLinesP(input, lines, 1, CV_PI / 180, 100, 100, 80);
	vector<Point2i> points;
	for (unsigned int i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		points.push_back(Point2i(l[0], l[1]));
		points.push_back(Point2i(l[2], l[3]));
		//cout << l[0] << ", " << l[1] << ", " << l[2] << ", " << l[3] << endl;
	}
	if (points.size() >= 2) {
		fitLine(points, yline, CV_DIST_L2, 0, 0.01, 0.01);
	} else {
		return Vec4i(1, 100, input.cols / 50, input.rows / 2);
	}
	return yline;
}

Vec4f LineDetection::calcStartEndWhiteLine(const Mat &input, Vec4f center) {
	vector<Vec4i> lines;
	Vec4f yline;

	HoughLinesP(input, lines, 1, CV_PI / 180, 100, 100, 80);
	vector<Point2i> points;
	for (unsigned int i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		if (l[0] > center[2] && l[2] > center[2]) {
			points.push_back(Point2i(l[0], l[1]));
			points.push_back(Point2i(l[2], l[3]));
		}
		//cout << l[0] << ", " << l[1] << ", " << l[2] << ", " << l[3] << endl;
	}
	if (points.size() >= 2) {
		fitLine(points, yline, CV_DIST_L2, 0, 0.01, 0.01);
	} else {
		return Vec4i(1, 100, input.cols * 49 / 50, input.rows / 2);
	}
	return yline;
}
