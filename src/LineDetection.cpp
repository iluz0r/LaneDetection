#include "LineDetection.h"

LineDetection::LineDetection() {

}

LineDetection::~LineDetection() {
// TODO Auto-generated destructor stub
}

// Questa maschera è utile quando lo spazio esterno alla pista è maggiore (in area) alla dimensione delle linee
// Il problema di questa maschera si presenta quando con la dilate non riusciamo a chiudere i buchi (delle linee)
// che si trovano all'interno della pista (praticamente si rischia di perdere la linea)
Mat LineDetection::calcBlackMask(const Mat &input) {
	//Controllo che la matrice in ingresso sia in HSV
	assert(input.type() == CV_8UC3);

	// Effettuo il threshold per una determinata scala di neri
	Mat blackThresh;
	inRange(input, Scalar(0, 0, 0), Scalar(180, 255, 100), blackThresh);
	showImg("Thrush black (inside calcBlackMask)", blackThresh);

	// Effettuo la dilatazione specificandone il kernel
	Mat mask;
	int k = 6;
	Mat kernel = getStructuringElement(MORPH_RECT,
			Size(k * 2 * DILATE_SIZE + 1, k * 2 * DILATE_SIZE + 1),
			Point(DILATE_SIZE, DILATE_SIZE));
	dilate(blackThresh, mask, kernel);

	// Effettuo l'erosione specificandone il kernel
	kernel = getStructuringElement(MORPH_RECT,
			Size(2 * ERODE_SIZE + 1, 2 * ERODE_SIZE + 1),
			Point(ERODE_SIZE, ERODE_SIZE));
	erode(mask, mask, kernel);

	return mask;
}

Mat LineDetection::calcYellowMask(const Mat &input, const Mat &blackMask) {
	// Effettuo il threshold per una determinata scala di gialli
	Mat yellowThresh;
	inRange(input, Scalar(19, 45, 100), Scalar(49, 255, 255), yellowThresh);
	showImg("thresh yellow (inside calcYellowMask)", yellowThresh);

	// Secondo me non serve la black mask!

	// Faccio la AND tra blackMask e yellowThresh
	Mat yellowMask;
	bitwise_and(blackMask, yellowThresh, yellowMask, noArray());
	showImg("yellowMask (inside calcYellowMask", yellowMask);

	// Effettuo la dilatazione specificandone il kernel.
	// La dilatazione serve per migliorare l'estrazione dell'edge quando si mette la maschera in AND con Canny.
	Mat kernel = getStructuringElement(MORPH_RECT,
			Size(2 * DILATE_SIZE + 1, 2 * DILATE_SIZE + 1),
			Point(DILATE_SIZE, DILATE_SIZE));
	//Mat yellowMask;
	dilate(yellowMask, yellowMask, kernel);

	return yellowMask;
}

Mat LineDetection::calcWhiteMask(const Mat &input, const Mat &blackMask) {
	// Effettuo il threshold per una determinata scala di bianchi
	Mat whiteThresh;
	inRange(input, Scalar(0, 0, 235), Scalar(255, 25, 255), whiteThresh);
	showImg("thresh white (inside calcWhiteMask)", whiteThresh);

	// Secondo me non serve la black mask!
	// Faccio la AND tra blackMask e whiteThresh
	Mat whiteMask;
	bitwise_and(blackMask, whiteThresh, whiteMask, noArray());
	showImg("whiteMask (inside calcWhiteMask)", whiteMask);

	// Effettuo la dilatazione specificandone il kernel
	// La dilatazione serve per migliorare l'estrazione dell'edge quando si mette la maschera in AND con Canny.
	Mat kernel = getStructuringElement(MORPH_RECT,
			Size(2 * DILATE_SIZE + 1, 2 * DILATE_SIZE + 1),
			Point(DILATE_SIZE, DILATE_SIZE));
	//Mat whiteMask;
	dilate(whiteMask, whiteMask, kernel);

	return whiteMask;
}

Mat LineDetection::calcRedMask(const Mat &input, const Mat &blackMask) {
	// Effettuo il threshold per una determinata scala di rossi
	Mat redThresh;
	inRange(input, Scalar(165, 100, 100), Scalar(185, 255, 255), redThresh);
	showImg("thresh red (inside calcRedMask)", redThresh);

	// Secondo me non serve la black mask!
	/*
	 // Faccio la AND tra blackMask e redThresh
	 Mat redMask;
	 bitwise_and(blackMask, redThresh, redMask, noArray());
	 showImg("redMask (inside calcRedMask)", redMask);
	 */

	// Effettuo la dilatazione specificandone il kernel
	// La dilatazione serve per migliorare l'estrazione dell'edge quando si mette la maschera in AND con Canny.
	Mat kernel = getStructuringElement(MORPH_RECT,
			Size(2 * DILATE_SIZE + 1, 2 * DILATE_SIZE + 1),
			Point(DILATE_SIZE, DILATE_SIZE));
	Mat redMask;
	dilate(redThresh, redMask, kernel);

	return redMask;
}

vector<Vec4f> LineDetection::detectLines(const Mat &input) {
	// Dichiaro il vettore di linee di output
	vector<Vec4f> linesOutput;

	// Designo l'area di interesse (la metà inferiore dell'immagine di ingresso) e la ritaglio inserendola in inputMod
	Rect roi = Rect(Point(0, input.rows / 5 * 3), Point(input.cols, input.rows));
	Mat inputMod = input(roi);

	assert(inputMod.type() == CV_8UC3);

	// Conversione da BGR a HSV
	Mat hsvImg;
	cvtColor(inputMod, hsvImg, CV_BGR2HSV);

	// Secondo me non serve la black mask, probabilmente andrà eliminata!
	// Black mask
	Mat blackMask = LineDetection::calcBlackMask(hsvImg);
	showImg("Black mask", blackMask);

	// Yellow threshold
	Mat yellowMask = LineDetection::calcYellowMask(hsvImg, blackMask);
	showImg("Yellow mask", yellowMask);

	// White threshold
	Mat whiteMask = LineDetection::calcWhiteMask(hsvImg, blackMask);
	showImg("White mask", whiteMask);

	// Red threshold
	Mat redMask = LineDetection::calcRedMask(hsvImg, blackMask);
	showImg("Red mask", redMask);

	// Applico Canny sull'immagine di partenza (che è in BGR)
	Mat grayImg, canny;
	cvtColor(inputMod, grayImg, CV_BGR2GRAY);
	Canny(grayImg, canny, 10, 100, 3);
	showImg("Canny", canny);

	// Copio in result l'immagine di partenza (metà inferiore) in BGR
	// Mi serve perché successivamente andrò a disegnarci le varie linee sopra
	Mat result = inputMod.clone();

	// Faccio AND tra Canny e la maschera del giallo (dilatata) per ottenere i bordi delle linee gialle
	Mat yellowEdges;
	bitwise_and(canny, yellowMask, yellowEdges, noArray());
	showImg("Contorni linee gialle", yellowEdges);

	// In startDirY ci sono 4 float che rappresentano [dx, dy, x0, y0] della linea gialla
	Vec4f startDirY = LineDetection::calcStartDirYellowLine(yellowEdges);
	linesOutput.push_back(startDirY);

	// Faccio AND tra Canny e la maschera del bianco (dilatata) per ottenere i bordi delle linee bianche
	Mat whiteEdges;
	bitwise_and(canny, whiteMask, whiteEdges, noArray());
	showImg("Contorni linee bianche", whiteEdges);

	// In startEndW ci sono 4 float che rappresentano [dx, dy, x0, y0] della linea bianca
	Vec4f startDirW = LineDetection::calcStartDirWhiteLine(whiteEdges,
			startDirY);
	linesOutput.push_back(startDirW);

	// Faccio AND tra Canny e la maschera del rosso (dilatata) per ottenere i bordi delle linee rosse
	Mat redEdges;
	bitwise_and(canny, redMask, redEdges, noArray());
	showImg("Contorni linee rosse", redEdges);

	// In startDirR ci sono 4 float che rappresentano [dx, dy, x0, y0] della linea rossa
	Vec4f startDirR = LineDetection::calcStartDirRedLine(redEdges);
	linesOutput.push_back(startDirR);

	// Disegno le linee su result
	LineDetection::detectAndShowLines(redEdges, result, Scalar(0, 0, 255));

	// Disegno le linee su result
	LineDetection::detectAndShowLines(whiteEdges, result, Scalar(0, 0, 0));

	// Disegno le linee su result
	LineDetection::detectAndShowLines(yellowEdges, result, Scalar(0, 255, 255));

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
	Point whiteStartPoint(startDirW[2], startDirW[3]), yellowStartPoint(
			startDirY[2], startDirY[3]);

	// Calcolo i coefficienti angolari di linea bianca e linea gialla
	float mw = startDirW[1] / startDirW[0], my = startDirY[1] / startDirY[0];

	// Calcolo la distanza tra P e le due rette (bianca e gialla)
	float distWhite = abs(
			P.x - (P.y - (whiteStartPoint.y - mw * whiteStartPoint.x)) / mw);
	float distYellow = abs(
			P.x - (P.y - (yellowStartPoint.y - my * yellowStartPoint.x)) / my);

	// Disegno su input: cerchio rosso (punto medio visuale), cerchio bianco (punto tra punto medio e retta bianca)
	// e cerchio giallo (punto tra punto medio e retta gialla)
	circle(input, P, 5, Scalar(0, 0, 255), 8, 1, 0);
	circle(input, Point(P.x + distWhite / 2, P.y), 5, Scalar(255, 255, 255), 8,
			1, 0);
	circle(input, Point(P.x - distYellow / 2, P.y), 5, Scalar(0, 255, 255), 8,
			1, 0);

	// Disegno la result con tutte le linee e cerchi
	imshow("LINE", input);
	waitKey(1);

	Vec4f startDirR = lines[2];
	// Se viene trovata la linea rossa il robot rallenta
	if (startDirR[0] != 1 && startDirR[1] != 100 && leftWheel >= 1
			&& rightWheel >= 1) {
		leftWheel *= 0.9;
		rightWheel *= 0.9;
	}

	// Rettifico la mia traiettoria valutando le distanze dalle rette bianche e gialle
	// Ho dei dubbi sul primo if!
	float threshold = 0.012, diff = (distWhite - distYellow) / input.cols;
	if (diff < -threshold) {
		leftWheel = 1;
		rightWheel = 1.5;
	} else if (diff > threshold) {
		leftWheel = 1.5;
		rightWheel = 1;
	}
}

void LineDetection::detectAndShowLines(const Mat &input, Mat &output,
		Scalar color) {
	vector<Vec4i> lines;
	HoughLinesP(input, lines, 1, CV_PI / 180 / 2, 40, 10, 10);

	for (size_t i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		line(output, Point(l[0], l[1]), Point(l[2], l[3]), color, 3, 4);
	}
}

Vec4f LineDetection::calcStartDirYellowLine(const Mat &yellowEdges) {
	// Detecto le linee usando Hough (questo perché nella yellowEdges potrebbero esserci contorni di
	// oggetti gialli esterni alla pista; con Hough andiamo ad estrapolare le linee, escludendo forme
	// diverse e quindi gli oggetti esterni alla pista)
	// L'unico caso sfigato è quello in cui ci sono linee gialle all'esterno della pista, ma questo
	// worst case non viene contemplato nemmeno da quelli del MIT
	vector<Vec4i> lines;
	HoughLinesP(yellowEdges, lines, 1, CV_PI / 180, 40, 10, 10);

	// Inserisco tutti i punti delle linee gialle in points
	vector<Point2i> points;
	for (unsigned int i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		points.push_back(Point2i(l[0], l[1]));
		points.push_back(Point2i(l[2], l[3]));
	}

	Vec4f yline;
	if (points.size() >= 2)
		fitLine(points, yline, CV_DIST_L2, 0, 0.01, 0.01);
	else
		yline = Vec4i(1, 100, yellowEdges.cols / 50, yellowEdges.rows / 2);
	return yline;
}

Vec4f LineDetection::calcStartDirWhiteLine(const Mat &whiteEdges,
		Vec4f center) {
	vector<Vec4i> lines;
	HoughLinesP(whiteEdges, lines, 1, CV_PI / 180, 40, 10, 10);

	// Inserisco tutti i punti delle linee bianche (a destra della carreggiata) in points
	vector<Point2i> points;
	for (unsigned int i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		if (l[0] > center[2] && l[2] > center[2]) {
			points.push_back(Point2i(l[0], l[1]));
			points.push_back(Point2i(l[2], l[3]));
		}
	}

	Vec4f wline;
	if (points.size() >= 2)
		fitLine(points, wline, CV_DIST_L2, 0, 0.01, 0.01);
	else
		wline = Vec4i(1, 100, whiteEdges.cols * 49 / 50, whiteEdges.rows / 2);
	return wline;
}

Vec4f LineDetection::calcStartDirRedLine(const Mat &redEdges) {
	// Detecto le linee usando Hough (questo perché nella redEdges potrebbero esserci contorni di
	// oggetti rossi esterni alla pista; con Hough andiamo ad estrapolare le linee, escludendo forme
	// diverse e quindi gli oggetti esterni alla pista)
	// L'unico caso sfigato è quello in cui ci sono linee rosse all'esterno della pista, ma questo
	// worst case non viene contemplato nemmeno da quelli del MIT
	vector<Vec4i> lines;
	HoughLinesP(redEdges, lines, 1, CV_PI / 180 * 90, 50, 10, 10);

	// Inserisco tutti i punti delle linee rosse in points
	vector<Point2i> points;
	for (unsigned int i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		points.push_back(Point2i(l[0], l[1]));
		points.push_back(Point2i(l[2], l[3]));
	}

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
