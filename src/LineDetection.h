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

#define VERBOSE false
#define DILATE_SIZE 2
#define ERODE_SIZE 8

using namespace cv;
using namespace std;

class LineDetection {
public:
	LineDetection();
	virtual ~LineDetection();
	static vector<Vec4f> detectLines(const Mat &img);
	static void calcAdjParams(const vector<Vec4f> &lines, Mat &input,
			float &leftWheel, float &rightWheel);
	static void detectAndShowLines(const Mat &input, Mat &output, Scalar color);
	static Vec4f calcStartDirYellowLine(const Mat &input);
	static Vec4f calcStartDirRedLine(const Mat &input);
	static Vec4f calcStartDirWhiteLine(const Mat &input, Vec4f center);
	static Mat calcBlackMask(const Mat &input);
	static Mat calcYellowMask(const Mat &input, const Mat &blackMask);
	static Mat calcWhiteMask(const Mat &input, const Mat &blackMask);
	static Mat calcRedMask(const Mat &input, const Mat &blackMask);
	static void showImg(String nameWindow, const Mat& mat);
};

#endif /* SRC_LINEDETECTION_H_ */
