/*
 * SignalDetect.h
 *
 *  Created on: Jul 12, 2017
 *      Author: virus
 */

#ifndef SRC_SIGNALDETECT_H_
#define SRC_SIGNALDETECT_H_

#include <core/core.hpp>
#include <core/mat.hpp>
#include <core/operations.hpp>
#include <core/types_c.h>
#include <imgproc/imgproc.hpp>
#include <imgproc/types_c.h>
#include <objdetect/objdetect.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "Classificatore.h"

#define SCALING_FACTOR				1.1
#define MIN_NEIGHBORS				10
#define SIGNAL_MIN_WIDTH			90
#define SIGNAL_MIN_HEIGHT			90
#define SIGNAL_MAX_WIDTH			800
#define SIGNAL_MAX_HEIGHT			800

class SignalDetect {
public:
	SignalDetect();
	virtual ~SignalDetect();
	int static detectAndClassifySignal(Mat& input,
			CascadeClassifier & SignalCascade, Classificatore& clas);
};

#endif /* SRC_SIGNALDETECT_H_ */
