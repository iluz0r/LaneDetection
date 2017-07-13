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
	static float detectLines(const Mat &img);
	static void detectAndShowLines(const Mat &input, Mat &output, Scalar color);
	static Vec4f calcStartEndYellowLine(const Mat &input);
	static Vec4f calcStartEndWhiteLine(const Mat &input, Vec4f center);
	static Mat calcBlackMask(const Mat &input);
	static Mat calcYellowMask(const Mat &input, const Mat &blackMask);
	static Mat calcWhiteMask(const Mat &input, const Mat &blackMask);
	static Mat calcRedMask(const Mat &input, const Mat &blackMask);
	static void showImg(String nameWindow, const Mat& mat);
};

#endif /* SRC_LINEDETECTION_H_ */
