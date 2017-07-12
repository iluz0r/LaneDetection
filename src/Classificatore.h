#include <contrib/contrib.hpp>
#include <core/core.hpp>
#include <core/core_c.h>
#include <core/mat.hpp>
#include <core/operations.hpp>
#include <core/types_c.h>
#include <dirent.h>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>
#include <imgproc/types_c.h>
#include <ml/ml.hpp>
#include <objdetect/objdetect.hpp>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace cv;

class CvKNearest;
class CvSVM;
struct CvSVMParams;

#ifndef CLASSIFICATORE_H_
#define CLASSIFICATORE_H_

#define NUMBER_OF_TO_MOMENTS			24
#define NUMBER_OF_HU_MOMENTS			7
#define NUMBER_OF_HISTOGRAM_FEATURES 	6
#define NUMBER_OF_EDGE_FEATURES			1
#define NUMBER_OF_HOG_FEATURES			99864
#define NUMBER_OF_LBP_FEATURES			256

#define USE_THIRD_ORDER_MOMENTS			0
#define USE_HU_MOMENTS					1
#define USE_FEATURE_EDGE 				1
#define USE_HISTOGRAM_FEATURES			1
#define USE_SPLIT_IMAGE_FOR_HISTOGRAM	1
#define USE_HOG_FEATURES				1
#define USE_LBP_FEATURES				1

#define PCA_VARIANCE					0.98
#define THRESH 							200

#define PARAMETER_KNN_K  				5;
#define PARAMETER_SVM_KERNEL 			CvSVM::LINEAR;
#define PARAMETER_SVM_TYPE 				CvSVM::C_SVC;

#define DATASET_HEIGHT_SIZE 			18
#define DATASET_WIDTH_SIZE				32
using namespace cv;
using namespace std;

class Classificatore {
private:
	int numberOfClass;
	int elementsByClass;
	int trainingPercentage;
	String path;
	//---- KNN----
	int K;
	CvKNearest* knn;
	//--- SVM ---
	CvSVM *SVM;
	CvSVMParams* params;
	//----
	Mat testData;
	Mat testClasses;
	Mat trainData;
	Mat trainClasses;
	vector<int>* errorsTest;
	vector<int>* errorsTraining;
	vector<String> name;
	int classifierType;
	unsigned int featuresCODE;

	void computeFeatures(Mat &img, vector<float> & fv);
	void showHistogram(Mat &img);

	void histogramLBP(const Mat &lbp, vector<float> &fv, const int numBins);
	void ELBP(const Mat &img, Mat &lbp, int radius, int neighbors);
	void OLBP(const Mat &img, Mat &lbp);

	void computeFeaturesDefault(Mat &img, vector<float> & fv);
	void computeFeaturesEdge(Mat &img, vector<float> &fv);
	void computeFeaturesHistogram(Mat &img, vector<float> &fv);
	void computeFeaturesHOG(Mat &img, vector<float> &fv);
	void computeFeaturesLBP(Mat &img, vector<float> &fv);

	bool isCorrectDecision(Mat &img, int classType);
	void splitimage(Mat &img, vector<Mat> &blocks);
	vector<vector<String> > getAllFileName(String sourcePath);
	String extractNameDirectory(String path);
	void train();
	float getDecision(Mat featureRow);
	unsigned int getDefaultConfiguration();
	void defaultInit();
public:
	static const int CLASSIFIER_TYPE_KNN = 0;
	static const int CLASSIFIER_TYPE_SVM = 1;
	static const unsigned int F_COD_THIRD_ORDER_MOMENTS = 1;
	static const unsigned int F_COD_HU_MOMENTS = 1 << 1;
	static const unsigned int F_COD_FEATURE_EDGE = 1 << 2;
	static const unsigned int F_COD_HISTOGRAM_FEATURES = 1 << 3;
	static const unsigned int F_COD_SPLIT_IMAGE_FOR_HISTOGRAM = (1 << 4)
			| (1 << 3);
	static const unsigned int F_COD_FEATURE_HOG = 1 << 5;
	static const unsigned int F_COD_FEATURE_LBP = 1 << 6;

	Classificatore();
	Classificatore(String pathOfClassifierBackup);
	Classificatore(String path, int numClass, int numElementByClass,
			int trainingPercentage);
	Classificatore(String path, int numClass, int numElementByClass,
			int trainingPercentage, int classifierType);
	Classificatore(String path, int numClass, int numElementByClass,
			int trainingPercentage, int classifierType,
			unsigned int featuresCODE);
	virtual ~Classificatore();
	float evaluateTrainigAccuracy();
	float evaluateTestAccuracy();
	void loadData();
	void normalizeData();
	void computePCA();
	void computeLDA();
	float getClassDecision(Mat &img);
	String getClassName(unsigned int classType);
	int getNumberOfClass();
	bool loadClassifier(String fileName);
	void storeClassifier(String fileName);
};

#endif /* CLASSIFICATORE_H_ */
