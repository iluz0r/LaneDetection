#ifndef SRC_LINEDETECTION_H_
#define SRC_LINEDETECTION_H_

#include <opencv2/core/core.hpp>
#include <opencv2/core/operations.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <stddef.h>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <climits>

#define VERBOSE false
#define DILATE_SIZE 4
#define CUT_RATIO 3/5

using namespace cv;
using namespace std;

class LineDetection {
public:
	LineDetection();
	virtual ~LineDetection();
	static vector<Vec4f> detectLines(const Mat &img);
	static void calcAdjParams(const vector<Vec4f> &lines, Mat &input,
			float &leftWheel, float &rightWheel);
	//static void detectAndShowLines(const Mat &input, Mat &output, Scalar color);
	static Vec4f calcStartDirYellowLine(const Mat &input);
	static Vec4f calcStartDirRedLine(const Mat &input);
	static Vec4f calcStartDirWhiteLine(const Mat &input, Vec4f center);
	static Mat calcYellowMask(const Mat &input);
	static Mat calcWhiteMask(const Mat &input);
	static Mat calcRedMask(const Mat &input);
	static void showImg(String nameWindow, const Mat& mat);
	static float calcDistRed(Vec4f Line, Point p);
	static float calcDist(Vec4f line, Point p);
};

#endif /* SRC_LINEDETECTION_H_ */
