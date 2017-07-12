#include "Classificatore.h"

Classificatore::Classificatore(String path, int numClass, int numElementByClass,
		int trainingPercentage, int classifierType, unsigned int featuresCODE) {
	this->numberOfClass = numClass;
	this->elementsByClass = numElementByClass;
	this->trainingPercentage = trainingPercentage;
	this->path = path;
	this->testData = Mat();
	this->testClasses = Mat();
	this->trainData = Mat();
	this->trainClasses = Mat();
	this->errorsTraining = new vector<int>();
	this->errorsTest = new vector<int>();
	this->classifierType = classifierType;
	switch (classifierType) {
	case CLASSIFIER_TYPE_SVM:
		this->params = new CvSVMParams();
		this->params->svm_type = PARAMETER_SVM_TYPE;
		this->params->kernel_type = PARAMETER_SVM_KERNEL;
		this->params->term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
		this->SVM = new CvSVM();
		break;

		default:
		this->K = PARAMETER_KNN_K;
		this->knn = new CvKNearest();
	}
	this->featuresCODE = featuresCODE;
}

Classificatore::Classificatore(String path, int numClass, int numElementByClass,
		int trainingPercentage, int classifierType) {
	this->numberOfClass = numClass;
	this->elementsByClass = numElementByClass;
	this->trainingPercentage = trainingPercentage;
	this->path = path;
	this->testData = Mat();
	this->testClasses = Mat();
	this->trainData = Mat();
	this->trainClasses = Mat();
	this->errorsTraining = new vector<int>();
	this->errorsTest = new vector<int>();
	this->classifierType = classifierType;
	switch (classifierType) {
	case CLASSIFIER_TYPE_SVM:
		this->params = new CvSVMParams();
		this->params->svm_type = PARAMETER_SVM_TYPE;
		this->params->kernel_type = PARAMETER_SVM_KERNEL;
		this->params->term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);
		this->SVM = new CvSVM();
		break;

		default:
		this->K = PARAMETER_KNN_K;
		this->knn = new CvKNearest();
	}
	this->featuresCODE = getDefaultConfiguration();
}

Classificatore::Classificatore(String path, int numClass, int numElementByClass,
		int trainingPercentage) {
	this->numberOfClass = numClass;
	this->elementsByClass = numElementByClass;
	this->trainingPercentage = trainingPercentage;
	this->path = path;
	this->K = PARAMETER_KNN_K
	;
	this->knn = new CvKNearest();
	this->testData = Mat();
	this->testClasses = Mat();
	this->trainData = Mat();
	this->trainClasses = Mat();
	this->errorsTraining = new vector<int>();
	this->errorsTest = new vector<int>();
	this->classifierType = CLASSIFIER_TYPE_KNN;
	this->featuresCODE = getDefaultConfiguration();
}

Classificatore::Classificatore(String pathOfClassifierBackup) {
	if (!loadClassifier(pathOfClassifierBackup))
		defaultInit();
}

Classificatore::Classificatore() {
	defaultInit();
}

void Classificatore::defaultInit() {
	this->numberOfClass = 2;
	this->elementsByClass = 10;
	this->trainingPercentage = 90;
	this->path = "File/DataSet";
	this->K = PARAMETER_KNN_K
	;
	this->knn = new CvKNearest();
	this->testData = Mat();
	this->testClasses = Mat();
	this->trainData = Mat();
	this->trainClasses = Mat();
	this->errorsTraining = new vector<int>();
	this->errorsTest = new vector<int>();
	this->classifierType = CLASSIFIER_TYPE_KNN;
	this->featuresCODE = getDefaultConfiguration();
}

Classificatore::~Classificatore() {
	switch (classifierType) {
	case CLASSIFIER_TYPE_SVM:
		delete SVM;
		delete params;
		break;

	default:
		delete knn;
	}
	delete errorsTest;
	delete errorsTraining;
}

void Classificatore::loadData() {
	unsigned int i, j;
	int l;
	int training_per_class = trainingPercentage * elementsByClass / 100;
	int test_per_class = (100 - trainingPercentage) * elementsByClass / 100;

	int num_of_features = 0;
	if (featuresCODE & F_COD_THIRD_ORDER_MOMENTS) {
		num_of_features += NUMBER_OF_TO_MOMENTS;
	}
	if (featuresCODE & F_COD_HU_MOMENTS) {
		num_of_features += NUMBER_OF_HU_MOMENTS;
	}
	if (featuresCODE & F_COD_HISTOGRAM_FEATURES) {
		if ((featuresCODE & F_COD_SPLIT_IMAGE_FOR_HISTOGRAM)
				== F_COD_SPLIT_IMAGE_FOR_HISTOGRAM) {
			num_of_features += 4 * NUMBER_OF_HISTOGRAM_FEATURES;
		} else {
			num_of_features += NUMBER_OF_HISTOGRAM_FEATURES;
		}
	}
	if (featuresCODE & F_COD_FEATURE_EDGE) {
		num_of_features += NUMBER_OF_EDGE_FEATURES;
	}
	if (featuresCODE & F_COD_FEATURE_HOG) {
		num_of_features += NUMBER_OF_HOG_FEATURES;
	}
	if (featuresCODE & F_COD_FEATURE_LBP) {
		num_of_features += NUMBER_OF_LBP_FEATURES;
	}

	trainData = Mat(training_per_class * numberOfClass, num_of_features,
	CV_32F);
	trainClasses = Mat(training_per_class * numberOfClass, 1, CV_32F);
	testData = Mat(test_per_class * numberOfClass, num_of_features, CV_32F);
	testClasses = Mat(test_per_class * numberOfClass, 1, CV_32F);

	vector<vector<String> > allFiles = getAllFileName(this->path);
	Mat sample, sample_gray;

	int sizeDir = allFiles.size();

	if (sizeDir != this->numberOfClass) {
		cout << "Numero di classi diverso dal numero di cartelle caricate! "
				<< sizeDir << " - " << numberOfClass << endl;
	}
	assert(sizeDir == this->numberOfClass);

	int sizeFile = allFiles[0].size();
	if (sizeFile != this->elementsByClass) {
		cout
				<< "Numero di elementi per classi diverso dal numero di file caricati!"
				<< sizeFile << " - " << elementsByClass << endl;
	}
	assert(sizeFile == this->elementsByClass);

	int training_count_per_class;
	int test_count_per_class;
	for (i = 0; i < allFiles.size(); i++) {
		training_count_per_class = 0;
		test_count_per_class = 0;
		vector<String> directory = allFiles[i];

		String name = extractNameDirectory(directory[0]);
		this->name.push_back(name);
//		cout << "Classe " << i << " = " << extractNameDirectory(directory[0])
//				<< endl;

		for (j = 0; j < directory.size(); j++) {
			String filePath = directory[j];
			sample = imread(filePath, IMREAD_COLOR);
			vector<float> features;
			computeFeatures(sample, features);
			if (training_count_per_class < training_per_class) {
				for (l = 0; l < num_of_features; l++) {
					trainData.at<float>(
							i * training_per_class + training_count_per_class,
							l) = features[l];
				}
				trainClasses.at<float>(
						i * training_per_class + training_count_per_class, 0) =
						i;
				training_count_per_class++;
			} else {
				for (l = 0; l < num_of_features; l++) {
					testData.at<float>(
							i * test_per_class + test_count_per_class, l) =
							features[l];
				}
				testClasses.at<float>(i * test_per_class + test_count_per_class,
						0) = i;
				test_count_per_class++;
			}
			//cout << i * training_per_class + training_count_per_class  << "(" << training_count_per_class << ")"<< endl;
		}
	}
	train();
}

void Classificatore::normalizeData() {
	int i;
	for (i = 0; i < trainData.cols; i++) {
		Mat mean;
		Mat sigma;
		meanStdDev(trainData.col(i), mean, sigma);
		trainData.col(i) = (trainData.col(i) - mean) / sigma;
		testData.col(i) = (testData.col(i) - mean) / sigma;
	}
}

void Classificatore::computePCA() {
	PCA pca(trainData, Mat(), CV_PCA_DATA_AS_ROW, PCA_VARIANCE);
	trainData = pca.project(trainData);
	cout << "Dimensionality after PCA = " << trainData.size() << endl;
	testData = pca.project(testData);
}

void Classificatore::computeLDA() {
	LDA lda(trainData, trainClasses);
	Mat trainProjections = lda.project(trainData);
	trainProjections.copyTo(trainData);
	trainData.convertTo(trainData, CV_32F);
	cout << "Dimensionality after LDA = " << trainData.size() << endl;
	Mat testProjections = lda.project(testData);
	testProjections.copyTo(testData);
	testData.convertTo(testData, CV_32F);
}

float Classificatore::evaluateTrainigAccuracy() {
	int i;
	int true_responses_tr = 0;
	for (i = 0; i < trainData.rows; i++) {
		float decision = getDecision(trainData.row(i));
		if (decision == trainClasses.at<float>(i)) {
			true_responses_tr++;
		} else {
			errorsTraining->push_back(i);
		}
	}
	return ((float) true_responses_tr) / trainData.rows * 100;
}

float Classificatore::evaluateTestAccuracy() {
	int i;
	int true_responses_tr = 0;
	for (i = 0; i < testData.rows; i++) {
		float decision = getDecision(testData.row(i));
		if (decision == testClasses.at<float>(i)) {
			true_responses_tr++;
		} else {
			errorsTest->push_back(i);
		}
	}
	return ((float) true_responses_tr) / testData.rows * 100;
}

void Classificatore::computeFeaturesDefault(Mat &img, vector<float> & fv) {
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat sample_threshold, sample_gray;
	cvtColor(img, sample_gray, CV_BGR2GRAY);
	threshold(sample_gray, sample_threshold, THRESH, 255, CV_THRESH_BINARY);
	Moments mom = moments(sample_threshold);
	if (featuresCODE & F_COD_THIRD_ORDER_MOMENTS) {
		// spatial moments
		fv.push_back(mom.m00);
		fv.push_back(mom.m10);
		fv.push_back(mom.m01);
		fv.push_back(mom.m20);
		fv.push_back(mom.m11);
		fv.push_back(mom.m02);
		fv.push_back(mom.m30);
		fv.push_back(mom.m21);
		fv.push_back(mom.m12);
		fv.push_back(mom.m03);
		// central moments
		fv.push_back(mom.mu20);
		fv.push_back(mom.mu11);
		fv.push_back(mom.mu02);
		fv.push_back(mom.mu30);
		fv.push_back(mom.mu21);
		fv.push_back(mom.mu12);
		fv.push_back(mom.mu03);
		// central normalized moments
		fv.push_back(mom.nu20);
		fv.push_back(mom.nu11);
		fv.push_back(mom.nu02);
		fv.push_back(mom.nu30);
		fv.push_back(mom.nu21);
		fv.push_back(mom.nu12);
		fv.push_back(mom.nu03);
	}
	if (featuresCODE & F_COD_HU_MOMENTS) {
		double hu[7];
		cv::HuMoments(mom, hu);
		int i;
		for (i = 0; i < 7; i++) {
			fv.push_back((float) hu[i]);
		}
	}
//	namedWindow("Display window", WINDOW_AUTOSIZE);
//	imshow("Display window", sample_threshold);
//	waitKey(-1);
}

void Classificatore::computeFeaturesEdge(Mat &img, vector<float> & fv) {
	RNG rng(12345);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat sample_threshold, sample_gray, canny_output, dilation_dst, erosion_dst,
			elementErosion, element;
	int erosion_size = 2, dilation_size = 2;

	cvtColor(img, sample_gray, CV_BGR2GRAY);

	/// Detect edges using canny
	Canny(sample_gray, canny_output, THRESH, 2 * THRESH, 3);

	// Applico l'erosione e la dilatazione per migliorare la detect dei contorni
	element = getStructuringElement(MORPH_RECT,
			Size(2 * dilation_size + 1, 2 * dilation_size + 1),
			Point(dilation_size, dilation_size));
	/// Apply the dilation operation
	dilate(canny_output, dilation_dst, element);

	erosion_dst, elementErosion = getStructuringElement(MORPH_RECT,
			Size(2 * erosion_size + 1, 2 * erosion_size + 1),
			Point(erosion_size, erosion_size));
	/// Apply the erosion operation
	erode(dilation_dst, erosion_dst, elementErosion);

	/// Find contours
	findContours(erosion_dst, contours, hierarchy, RETR_TREE,
			CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Draw contours
	float count = 0;

	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	for (unsigned int i = 0; i < contours.size(); i++) {
		if (hierarchy[i][3] != -1) {
			// random colour
			Scalar colour((rand() & 255), (rand() & 255), (rand() & 255));
			drawContours(drawing, contours, i, colour);
			count++;
		}
	}

	//	//  Get the mass centers:
	//	Point2f mc;
	//	mc = Point2f(mom.m10 / mom.m00, mom.m01 / mom.m00);
	//	circle(drawing, mc, 4, Scalar(0, 255, 255), -1, 8, 0);

	fv.push_back(count);
//	namedWindow("Origin", WINDOW_AUTOSIZE);
//	imshow("Origin", img);
//	namedWindow("Contours", WINDOW_AUTOSIZE);
//	imshow("Contours", drawing);
//	waitKey(-1);
}

void Classificatore::computeFeatures(Mat &img, vector<float> & fv) {
	if (featuresCODE & F_COD_FEATURE_EDGE)
		computeFeaturesEdge(img, fv);
	if ((featuresCODE & F_COD_HU_MOMENTS)
			|| (featuresCODE & F_COD_THIRD_ORDER_MOMENTS))
		computeFeaturesDefault(img, fv);
	if (featuresCODE & F_COD_HISTOGRAM_FEATURES) {
		if ((featuresCODE & F_COD_SPLIT_IMAGE_FOR_HISTOGRAM)
				== F_COD_SPLIT_IMAGE_FOR_HISTOGRAM) {
			vector<Mat> imgs;
			splitimage(img, imgs);
			for (unsigned int i = 0; i < imgs.size(); i++) {
				computeFeaturesHistogram(imgs[i], fv);
			}
		} else {
			computeFeaturesHistogram(img, fv);
		}
	}
	if (featuresCODE & F_COD_FEATURE_HOG)
		computeFeaturesHOG(img, fv);
	if (featuresCODE & F_COD_FEATURE_LBP)
		computeFeaturesLBP(img, fv);
}

void Classificatore::computeFeaturesHOG(Mat &img, vector<float> &fv) {
	// Dipende dall'input devo farlo?
	Mat resized;
	resize(img, resized, Size(DATASET_WIDTH_SIZE, DATASET_HEIGHT_SIZE), 0, 0,
			INTER_LINEAR);
	assert(((img.rows % 3) == 0) && ((img.cols % 4) == 0));
	HOGDescriptor hog(Size(DATASET_WIDTH_SIZE, DATASET_HEIGHT_SIZE),
			Size(16, 16), Size(2, 2), Size(8, 8), 9, -1, 0.2, true,
			HOGDescriptor::DEFAULT_NLEVELS);
	vector<float> descriptors;
	hog.compute(resized, descriptors);
	for (unsigned int i = 0; i < descriptors.size(); i++) {
		fv.push_back(descriptors[i]);
	}
}

void Classificatore::histogramLBP(const Mat &lbp, vector<float> &fv,
		const int numBins) {
	fv = vector<float>(numBins);
	for (int i = 0; i < lbp.rows; i++) {
		for (int j = 0; j < lbp.cols; j++) {
			int bin = lbp.at<unsigned char>(i, j);
			fv[bin]++;
		}
	}
}

void Classificatore::ELBP(const Mat &img, Mat &lbp, int radius, int neighbors) {
	neighbors = max(min(neighbors, 31), 1); // set bounds
	lbp = Mat::zeros(img.rows - 2 * radius, img.cols - 2 * radius, CV_32SC1);
	for (int n = 0; n < neighbors; n++) {
		// sample points
		float x = static_cast<float>(radius)
				* cos(2.0 * M_PI * n / static_cast<float>(neighbors));
		float y = static_cast<float>(radius)
				* -sin(2.0 * M_PI * n / static_cast<float>(neighbors));
		// relative indices
		int fx = static_cast<int>(floor(x));
		int fy = static_cast<int>(floor(y));
		int cx = static_cast<int>(ceil(x));
		int cy = static_cast<int>(ceil(y));
		// fractional part
		float ty = y - fy;
		float tx = x - fx;
		// set interpolation weights
		float w1 = (1 - tx) * (1 - ty);
		float w2 = tx * (1 - ty);
		float w3 = (1 - tx) * ty;
		float w4 = tx * ty;
		// iterate through your data
		for (int i = radius; i < img.rows - radius; i++) {
			for (int j = radius; j < img.cols - radius; j++) {
				float t = w1 * img.at<unsigned char>(i + fy, j + fx)
						+ w2 * img.at<unsigned char>(i + fy, j + cx)
						+ w3 * img.at<unsigned char>(i + cy, j + fx)
						+ w4 * img.at<unsigned char>(i + cy, j + cx);
				// we are dealing with floating point precision, so add some little tolerance
				lbp.at<unsigned int>(i - radius, j - radius) += ((t
						> img.at<unsigned char>(i, j))
						&& (abs(t - img.at<unsigned char>(i, j))
								> std::numeric_limits<float>::epsilon())) << n;
			}
		}
	}
}

void Classificatore::OLBP(const Mat &img, Mat &lbp) {
	lbp = Mat::zeros(img.rows - 2, img.cols - 2, CV_8UC1);
	for (int i = 1; i < img.rows - 1; i++) {
		for (int j = 1; j < img.cols - 1; j++) {
			float center = img.at<unsigned char>(i, j);
			unsigned char code = 0;
			code |= (img.at<unsigned char>(i - 1, j - 1) > center) << 7;
			code |= (img.at<unsigned char>(i - 1, j) > center) << 6;
			code |= (img.at<unsigned char>(i - 1, j + 1) > center) << 5;
			code |= (img.at<unsigned char>(i, j + 1) > center) << 4;
			code |= (img.at<unsigned char>(i + 1, j + 1) > center) << 3;
			code |= (img.at<unsigned char>(i + 1, j) > center) << 2;
			code |= (img.at<unsigned char>(i + 1, j - 1) > center) << 1;
			code |= (img.at<unsigned char>(i, j - 1) > center) << 0;
			lbp.at<unsigned char>(i - 1, j - 1) = code;
		}
	}
}

void Classificatore::computeFeaturesLBP(Mat &img, vector<float> &fv) {
	// Probabilmente fare resize del campione
	Mat sample_gray;
	cvtColor(img, sample_gray, CV_BGR2GRAY);
	// Smooth per migliorare il risultato
	GaussianBlur(sample_gray, sample_gray, Size(7, 7), 5, 3, BORDER_CONSTANT);
	Mat lbp;
	// LBP originale
	OLBP(sample_gray, lbp);
	// LBP esteso
	// ELBP(sample_gray, lbp, 4, 4); // Fare prove facendo variare radius e neightbors tra 2, 4, 8, 12, 16.. non oltre
	// Normalizzo da 0 a 255 in modo da avere sempre e solo 256 features
	normalize(lbp, lbp, 0, 255, NORM_MINMAX, CV_8UC1);
	histogramLBP(lbp, fv, NUMBER_OF_LBP_FEATURES);
}

void Classificatore::splitimage(Mat &img, vector<Mat> &blocks) {
	int dimCol = (img.cols / 2);
	int dimRow = (img.rows / 2);
	blocks.push_back(img(Rect(0, 0, dimCol, dimRow)).clone());
	blocks.push_back(img(Rect(dimCol, 0, (img.cols - dimCol), dimRow)).clone());
	blocks.push_back(img(Rect(0, dimRow, dimCol, (img.rows - dimRow))).clone());
	blocks.push_back(
			img(Rect(dimCol, dimRow, (img.cols - dimCol), (img.rows - dimRow))).clone());
}

void Classificatore::computeFeaturesHistogram(Mat &img, vector<float> &fv) {
	Scalar mean_, stddev;
	meanStdDev(img, mean_, stddev);
	for (int i = 0; i < 3; i++) {
		//cout << "media: " << mean_[i] << " varianza: " << stddev[i] << endl;
		fv.push_back((float) mean_[i]);
		fv.push_back((float) stddev[i]);
	}
	//showHistogram(img);
}

void Classificatore::showHistogram(Mat &img) {

	/// Separate the image in 3 places ( B, G and R )
	vector<Mat> bgr_planes;
	split(img, bgr_planes);

	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true;
	bool accumulate = false;

	Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange,
			uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange,
			uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange,
			uniform, accumulate);

	// Draw the histograms for B, G and R
	int hist_w = 512;
	int hist_h = 400;
	int bin_w = cvRound((double) hist_w / histSize);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	/// Draw for each channel
	for (int i = 1; i < histSize; i++) {
		line(histImage,
				Point(bin_w * (i - 1),
						hist_h - cvRound(b_hist.at<float>(i - 1))),
				Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
				Scalar(255, 0, 0), 2, 8, 0);
		line(histImage,
				Point(bin_w * (i - 1),
						hist_h - cvRound(g_hist.at<float>(i - 1))),
				Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
				Scalar(0, 255, 0), 2, 8, 0);
		line(histImage,
				Point(bin_w * (i - 1),
						hist_h - cvRound(r_hist.at<float>(i - 1))),
				Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
				Scalar(0, 0, 255), 2, 8, 0);
	}
	namedWindow("origin", WINDOW_AUTOSIZE);
	imshow("origin", img);
	namedWindow("Histogram", WINDOW_AUTOSIZE);
	imshow("Histogram", histImage);
	waitKey(-1);
}

vector<vector<String> > Classificatore::getAllFileName(String sourcePath) {
	vector<vector<String> > listDir;
	stringstream path;
	DIR *dp, *dp2;
	struct dirent *ep;
	struct dirent *ep2;

	dp = opendir(sourcePath.c_str());
	if (dp == NULL) {
		cout << "Path non valido!" << endl;
		return listDir;
	}
	while ((ep = readdir(dp))) {
		if ((strcmp(ep->d_name, ".") != 0) && (strcmp(ep->d_name, "..") != 0)) {
			vector<String> directory;
			path.str("");
			path << sourcePath << "/" << ep->d_name;
			String relPath = path.str();
			dp2 = opendir(relPath.c_str());
			if (dp2 != NULL) {
				while ((ep2 = readdir(dp2))) {
					if ((strcmp(ep2->d_name, ".") != 0)
							&& (strcmp(ep2->d_name, "..") != 0)) {
						path.str("");
						path << relPath << "/" << ep2->d_name;
						String file = path.str();
						directory.push_back(file);
						//cout << "file name: " << file << endl;
					}
				}
				closedir(dp2);
			} else {
				cout << "errore durante la scansione delle directory" << endl;
			}
			listDir.push_back(directory);
		}
	}
	closedir(dp);
	return listDir;
}

bool Classificatore::isCorrectDecision(Mat &img, int classType) {
	vector<float> features;
	computeFeatures(img, features);
	Mat testData = Mat(1, features.size(), CV_32F);

	for (unsigned int l = 0; l < features.size(); l++) {
		testData.at<float>(0, l) = features[l];
	}
	float decision = getDecision(testData.row(0));
	cout << "decision: " << decision << endl;
	return (decision == classType);
}

float Classificatore::getDecision(Mat featureRow) {
	float result;
	switch (this->classifierType) {
	case CLASSIFIER_TYPE_SVM:
		result = SVM->predict(featureRow);
		break;
	default:
		result = knn->find_nearest(featureRow, K);
	}
	return result;
}

float Classificatore::getClassDecision(Mat &img) {
	vector<float> features;
	computeFeatures(img, features);
	Mat testData = Mat(1, features.size(), CV_32F);

	for (unsigned int l = 0; l < features.size(); l++) {
		testData.at<float>(0, l) = features[l];
	}
	return getDecision(testData.row(0));
}

String Classificatore::extractNameDirectory(String path) {
	int start = path.find("/"), old;
	int end = path.find("/", start + 1);
	old = end;
	while (old != path.npos) {
		start = end + 1;
		end = old;
		old = path.find("/", end + 1);
	}
	return path.substr(start, (end - start));
}

String Classificatore::getClassName(unsigned int classType) {
	if (classType < 0 || classType > this->name.size())
		return "";
	return this->name.at(classType);
}

void Classificatore::train() {
	switch (this->classifierType) {
	case CLASSIFIER_TYPE_SVM:
		this->SVM->train(trainData, trainClasses, Mat(), Mat(), *params);
		break;
	default:
		knn->train(trainData, trainClasses);
	}
}

unsigned int Classificatore::getDefaultConfiguration() {
	unsigned int mask = 0;

	mask |= USE_THIRD_ORDER_MOMENTS << 0;
	mask |= USE_HU_MOMENTS << 1;
	mask |= USE_FEATURE_EDGE << 2;
	mask |= USE_HISTOGRAM_FEATURES << 3;
	mask |= USE_SPLIT_IMAGE_FOR_HISTOGRAM << 4;
	mask |= USE_HOG_FEATURES << 5;

	return mask;
}

int Classificatore::getNumberOfClass() {
	return this->numberOfClass;
}

bool Classificatore::loadClassifier(String fileName) {
	// DA fare
	return false;
}
void Classificatore::storeClassifier(String fileName) {
	// Da fare
}
