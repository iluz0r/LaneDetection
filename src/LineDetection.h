/*
 * LineDection.h
 *
 *  Created on: 10 lug 2017
 *      Author: angelo
 */

#ifndef SRC_LINEDETECTION_H_
#define SRC_LINEDETECTION_H_

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

class LineDetection {
public:
	LineDetection();
	virtual ~LineDetection();
	static float detectLines(const Mat &img);
	static void detectAndShowLines(const Mat &input, Mat &output, Scalar color);
	static Vec4f calcStartEndYellowLine(const Mat &input);
	static Vec4f calcStartEndWhiteLine(const Mat &input, Vec4f center);
};

#endif /* SRC_LINEDETECTION_H_ */
