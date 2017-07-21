#include "LineDetection.h"

LineDetection::LineDetection() {

}

LineDetection::~LineDetection() {
// TODO Auto-generated destructor stub
}

Mat LineDetection::calcYellowMask(const Mat &input) {
	// Effettuo il threshold per una determinata scala di gialli
	Mat yellowThresh;
	inRange(input, Scalar(8, 25, 160), Scalar(45, 255, 255), yellowThresh);

	// Effettuo una apertura per eliminare il rumore di tipo sale (specie sulle linee bianche)
	Mat kernel = getStructuringElement(MORPH_ELLIPSE,
			Size(2 * 4 + 1, 2 * 4 + 1), Point(4, 4));
	morphologyEx(yellowThresh, yellowThresh, 2, kernel);
	// imshow("yellowThresh  + Opening", yellowThresh);

	// Effettuo la dilatazione specificandone il kernel.
	// La dilatazione serve per migliorare l'estrazione dell'edge quando si mette la maschera in AND con Canny.
	kernel = getStructuringElement(MORPH_RECT,
			Size(2 * DILATE_SIZE + 1, 2 * DILATE_SIZE + 1),
			Point(DILATE_SIZE, DILATE_SIZE));
	dilate(yellowThresh, yellowThresh, kernel);

	return yellowThresh;
}

Mat LineDetection::calcWhiteMask(const Mat &input) {
	/*
	 Mat inputYuv;
	 cvtColor(input, inputYuv, CV_BGR2YUV);
	 // Effettuo il threshold per una determinata scala di bianchi
	 Mat firstWhiteThresh;
	 inRange(inputYuv, Scalar(160,110,110), Scalar(255,150,150), firstWhiteThresh);
	 //imshow("firstWhiteThresh", firstWhiteThresh);

	 Mat inputLuv;
	 cvtColor(input, inputLuv, CV_BGR2Luv);

	 // Effettuo il threshold per una determinata scala di bianchi
	 Mat firstWhiteThresh;
	 inRange(inputLuv, Scalar(180, 80, 110), Scalar(255, 120, 150),
	 firstWhiteThresh);
	 //imshow("firstWhiteThresh", firstWhiteThresh);
	 */

	Mat whiteThresh;
	inRange(input, Scalar(0, 0, 180), Scalar(255, 25, 255), whiteThresh);
	//imshow("secondWhiteThresh", secondWhiteThresh);

	// Effettuo una apertura per eliminare il rumore di tipo sale (specie sulle linee gialle)
	Mat kernel = getStructuringElement(MORPH_ELLIPSE,
			Size(2 * 4 + 1, 2 * 4 + 1), Point(4, 4));
	morphologyEx(whiteThresh, whiteThresh, 2, kernel);
	//imshow("whiteThresh  + Opening", whiteThresh);

	// Effettuo la dilatazione specificandone il kernel
	// La dilatazione serve per migliorare l'estrazione dell'edge quando si mette la maschera in AND con Canny.
	kernel = getStructuringElement(MORPH_RECT,
			Size(2 * DILATE_SIZE + 1, 2 * DILATE_SIZE + 1),
			Point(DILATE_SIZE, DILATE_SIZE));
	dilate(whiteThresh, whiteThresh, kernel);
	//imshow("Dilated (whiteThresh + Opening)", whiteThresh);

	return whiteThresh;
}

Mat LineDetection::calcRedMask(const Mat &input) {
	// Effettuo il threshold per una determinata scala di rossi
	Mat redThresh;
	inRange(input, Scalar(140, 100, 100), Scalar(195, 255, 255), redThresh);
	showImg("thresh red (inside calcRedMask)", redThresh);

	// Effettuo la dilatazione specificandone il kernel
	// La dilatazione serve per migliorare l'estrazione dell'edge quando si mette la maschera in AND con Canny.
	Mat kernel = getStructuringElement(MORPH_RECT,
			Size(2 * DILATE_SIZE + 1, 2 * DILATE_SIZE + 1),
			Point(DILATE_SIZE, DILATE_SIZE));
	dilate(redThresh, redThresh, kernel);

	return redThresh;
}

vector<Vec4f> LineDetection::detectLines(const Mat &inputMod) {
	// Dichiaro il vettore di linee di output
	vector<Vec4f> linesOutput;

	assert(inputMod.type() == CV_8UC3);

	// Conversione da BGR a HSV
	Mat hsvImg;
	cvtColor(inputMod, hsvImg, CV_BGR2HSV);

	// Yellow threshold
	Mat yellowMask = LineDetection::calcYellowMask(hsvImg);
	showImg("Yellow mask", yellowMask);

	// White threshold
	Mat whiteMask = LineDetection::calcWhiteMask(hsvImg);
	showImg("White mask", whiteMask);

	// Red threshold
	Mat redMask = LineDetection::calcRedMask(hsvImg);
	showImg("Red mask", redMask);

	// Applico Canny sull'immagine di partenza (in Gray). I valori di Canny devono essere 1:2 o 1:3
	Mat grayImg, canny;
	cvtColor(inputMod, grayImg, CV_BGR2GRAY);
	// Rimuovo del rumore con il blur gaussiano
	GaussianBlur(grayImg, grayImg, Size(5, 5), 0);
	// Forse per la linea rossa serve fare un altro Canny con parametri differenti. I parametri devono essere sempre 1:3.
	Canny(grayImg, canny, 60, 180, 3);
	//imshow("Canny", canny);
	showImg("Canny", canny);

	// Copio in result l'immagine di partenza (metà inferiore) in BGR
	// Mi serve perché successivamente andrò a disegnarci le varie linee sopra
	Mat result = inputMod.clone();

	// Faccio AND tra Canny e la maschera del giallo (threshold dilatato) per ottenere i bordi delle linee gialle
	Mat yellowEdges;
	bitwise_and(canny, yellowMask, yellowEdges);
	// imshow("Yellow contours", yellowEdges);
	showImg("Contorni linee gialle", yellowEdges);

	// In startDirY ci sono 4 float che rappresentano [dx, dy, x0, y0] della linea gialla
	Vec4f startDirY = LineDetection::calcStartDirYellowLine(yellowEdges);
	line(result,
			Point(200 * startDirY[0] + startDirY[2],
					200 * startDirY[1] + startDirY[3]),
			Point(startDirY[2], startDirY[3]), Scalar(255, 100, 0), 3, 4);
	linesOutput.push_back(startDirY);

	// Faccio AND tra Canny e la maschera del bianco (dilatata) per ottenere i bordi delle linee bianche
	Mat whiteEdges;
	bitwise_and(canny, whiteMask, whiteEdges);
	//imshow("White contours", whiteEdges);
	showImg("Contorni linee bianche", whiteEdges);

	// In startEndW ci sono 4 float che rappresentano [dx, dy, x0, y0] della linea bianca
	Vec4f startDirW = LineDetection::calcStartDirWhiteLine(whiteEdges,
			startDirY);
	line(result,
			Point(200 * startDirW[0] + startDirW[2],
					200 * startDirW[1] + startDirW[3]),
			Point(startDirW[2], startDirW[3]), Scalar(255, 100, 255), 3, 4);
	linesOutput.push_back(startDirW);

	Mat cannyRed;
	Canny(grayImg, cannyRed, 20, 60, 3);
	//imshow("canny red", cannyRed);

	// Faccio AND tra Canny e la maschera del rosso (dilatata) per ottenere i bordi delle linee rosse
	Mat redEdges;
	bitwise_and(cannyRed, redMask, redEdges);
	showImg("Contorni linee rosse", redEdges);
	//imshow("Red contours", redEdges);

	// In startDirR ci sono 4 float che rappresentano [dx, dy, x0, y0] della linea rossa
	Vec4f startDirR = LineDetection::calcStartDirRedLine(redEdges);
	line(result,
			Point(200 * startDirR[0] + startDirR[2],
					200 * startDirR[1] + startDirR[3]),
			Point(startDirR[2], startDirR[3]), Scalar(255, 255, 255), 3, 4);
	linesOutput.push_back(startDirR);

	imshow("Result", result);
	waitKey(1);

	return linesOutput;
}

// Calcola i parametri dei motori per correggere la traiettoria
void LineDetection::calcAdjParams(const vector<Vec4f> &lines, Mat &input,
		float &leftWheel, float &rightWheel) {

	// Scelgo un punto P che è il punto medio della mia visuale
	Point P(input.cols / 2, input.rows / 4 * 3);

	// Considero punto di inizio della retta gialla e punto di inizio della retta bianca
	Vec4f startDirY = lines[0];
	Vec4f startDirW = lines[1];
	Vec4f startDirR = lines[2];
	Point whiteStartPoint(startDirW[2], startDirW[30]), yellowStartPoint(
			startDirY[2], startDirY[3]);
	/*
	 // Calcolo i coefficienti angolari di linea bianca e linea gialla
	 float mw = startDirW[1] / startDirW[0], my = startDirY[1] / startDirY[0];

	 // Calcolo la distanza tra P e le due rette (bianca e gialla)
	 float distWhite = abs(
	 P.x - (P.y - (whiteStartPoint.y - mw * whiteStartPoint.x)) / mw);
	 float distYellow = abs(
	 P.x - (P.y - (yellowStartPoint.y - my * yellowStartPoint.x)) / my);

	 // Disegno su input: cerchio rosso (punto medio visuale), cerchio bianco (punto tra punto medio e retta bianca)
	 // e cerchio giallo (punto tra punto medio e retta gialla)
	 */
	float distWhite = calcDist(startDirW, P);
	float distYellow = calcDist(startDirY, P);

	circle(input, P, 5, Scalar(0, 0, 255), 8, 1, 0);
	circle(input, Point(P.x + distWhite / 2, P.y), 5, Scalar(255, 255, 255), 8,
			1, 0);
	circle(input, Point(P.x - distYellow / 2, P.y), 5, Scalar(0, 255, 255), 8,
			1, 0);

	// Rettifico la mia traiettoria valutando le distanze dalle rette bianche e gialle
	// Ho dei dubbi sul primo if!
	float threshold = 20, diff = (distWhite - distYellow) / input.cols;
	diff *= 100;
	float softThreshold = 30;

	leftWheel = 0.5;
	rightWheel = 0.5;
	if (diff < -threshold) {
		if (diff < -softThreshold) {
			leftWheel = 0;
			rightWheel = 0.5;
		} else {
			leftWheel = 0.2;
			rightWheel = 0.5;
		}
	} else if (diff > threshold) {
		if (diff > softThreshold) {
			leftWheel = 0.5;
			rightWheel = 0;
		} else {
			leftWheel = 0.5;
			rightWheel = 0.2;
		}
	}
	// Disegno la result con tutte le linee e cerchi
	imshow("LINE", input);
	waitKey(0);
}

Vec4f LineDetection::calcStartDirYellowLine(const Mat &yellowEdges) {
	// Detecto le linee usando Hough (questo perché nella yellowEdges potrebbero esserci contorni di
	// oggetti gialli esterni alla pista; con Hough andiamo ad estrapolare le linee, escludendo forme
	// diverse e quindi gli oggetti esterni alla pista)
	// L'unico caso sfigato è quello in cui ci sono linee gialle all'esterno della pista
	vector<Vec4i> lines;
	HoughLinesP(yellowEdges, lines, 1, CV_PI / 180, 20, 10, 20);

//	Mat houghLines = Mat::zeros(600, 600, CV_8UC3);

	// Inserisco tutti i punti delle linee gialle in points
	vector<Point2i> points;
	for (unsigned int i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
//		line(houghLines, Point(l[0], l[1]), Point(l[2], l[3]),
//				Scalar(255, 255, 255), 3, 4);
		float m = ((float) (l[3] - l[1])) / (l[2] - l[0]);
		if (m < 0) {
			points.push_back(Point2i(l[0], l[1]));
			points.push_back(Point2i(l[2], l[3]));
		}
	}
	//imshow("Hough yellow", houghLines);

	Vec4f yline;
	if (points.size() >= 2)
		fitLine(points, yline, CV_DIST_L2, 0, 0.01, 0.01);
	else
		yline = Vec4i(1, 100, 0, yellowEdges.rows / 2);
	return yline;
}

Vec4f LineDetection::calcStartDirWhiteLine(const Mat &whiteEdges,
		Vec4f center) {
	vector<Vec4i> lines;
	HoughLinesP(whiteEdges, lines, 1, CV_PI / 180, 20, 10, 20);

	//Mat houghLines = Mat::zeros(600, 600, CV_8UC3);

	// Inserisco tutti i punti delle linee bianche (a destra della linea gialla + distFromYLine) in points
	int distFromYLine = 20;
	vector<Point2i> points;
	for (unsigned int i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		float m = INT_MAX;
		if (l[2] - l[0] != 0) {
			m = ((float) (l[3] - l[1])) / (l[2] - l[0]);
		}

		if (l[0] > (center[2] + distFromYLine)
				&& l[2] > (center[2] + distFromYLine)
				&& (m > 0.02 || m < -0.02)) {
//			line(houghLines, Point(l[0], l[1]), Point(l[2], l[3]),
//					Scalar(255, 255, 255), 3, 4);
			points.push_back(Point2i(l[0], l[1]));
			points.push_back(Point2i(l[2], l[3]));
		}
	}

	//imshow("Hough white", houghLines);
	Vec4f wline;
	if (points.size() >= 2)
		fitLine(points, wline, CV_DIST_L2, 0, 0.01, 0.01);
	else
		wline = Vec4i(1, 100, whiteEdges.cols, whiteEdges.rows / 2);
	return wline;
}

Vec4f LineDetection::calcStartDirRedLine(const Mat &redEdges) {
	// Detecto le linee usando Hough (questo perché nella redEdges potrebbero esserci contorni di
	// oggetti rossi esterni alla pista; con Hough andiamo ad estrapolare le linee, escludendo forme
	// diverse e quindi gli oggetti esterni alla pista)
	// L'unico caso sfigato è quello in cui ci sono linee rosse all'esterno della pista, ma questo
	// worst case non viene contemplato nemmeno da quelli del MIT
	vector<Vec4i> lines;
	HoughLinesP(redEdges, lines, 1, CV_PI / 180 * 90, 20, 10, 20);

//	Mat houghLines = Mat::zeros(600, 600, CV_8UC3);

	// Inserisco tutti i punti delle linee rosse in points
	vector<Point2i> points;
	for (unsigned int i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		points.push_back(Point2i(l[0], l[1]));
		points.push_back(Point2i(l[2], l[3]));
//		line(houghLines, Point(l[0], l[1]), Point(l[2], l[3]),
//				Scalar(255, 255, 255), 3, 4);
	}

	// imshow("Hough red",houghLines);

	Vec4f rline;
	if (points.size() >= 2)
		fitLine(points, rline, CV_DIST_L2, 0, 0.01, 0.01);
	else
		rline = Vec4i(1, 100, redEdges.cols / 50, redEdges.rows / 2);
	return rline;
}

void LineDetection::showImg(String nameWindow, const Mat &mat) {
	if (VERBOSE) {
		imshow(nameWindow, mat);
	}
}

float LineDetection::calcDistRed(Vec4f Line, Point p) {
	if (Line[0] == 1 && Line[1] == 100)
		return -1;
	float m = Line[1] / Line[0];
	if (m == 0) {
		return p.y - Line[3];
	}
	float q = Line[3] - m * Line[2];
	float qp = 1 / m * p.x + p.y;
	float rx = 0, ry = 0;
	rx = ((qp - q) / (m * m + 1)) * m;
	ry = m * rx + q;
	return sqrt(pow((p.x - rx), 2) + pow((p.y - ry), 2));

}

float LineDetection::calcDist(Vec4f Line, Point p) {

	float m = Line[1] / Line[0];
	if (m == 0) {
		return p.y - Line[3];
	}
	float q = Line[3] - m * Line[2];
	float qp = 1 / m * p.x + p.y;
	float rx = 0, ry = 0;
	rx = ((qp - q) / (m * m + 1)) * m;
	ry = m * rx + q;
	return sqrt(pow((p.x - rx), 2) + pow((p.y - ry), 2));

}
