/*
 * LineDection.h
 *
 *  Created on: 10 lug 2017
 *      Author: angelo
 */

#ifndef SRC_LINEDETECTION_H_
#define SRC_LINEDETECTION_H_

#define DILATE_SIZE 2
#define ERODE_SIZE 8

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <core/core.hpp>
#include <core/mat.hpp>
#include <core/operations.hpp>
#include <core/types_c.h>
#include <imgproc/types_c.h>
#include <stddef.h>
#include <iostream>

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
	static Mat calcBlackMask(const Mat &input);
	static Mat calcYellowMask(const Mat &input, const Mat &blackMask);
	static Mat calcWhiteMask(const Mat &input, const Mat &blackMask);
	static Mat calcRedMask(const Mat &input, const Mat &blackMask);
};

#endif /* SRC_LINEDETECTION_H_ */
