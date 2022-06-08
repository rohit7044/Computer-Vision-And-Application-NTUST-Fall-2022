// For File reading and Data manipulation including matrix multiplication
#include <math.h>
// For image manipulation
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

int main() {
	//3 Rectangle Co-ordinates
	//Since x,y,z are defined on 50 mm repectively in each direction
	// then persquare dimension is roughly 10mm,10mm
	std::vector<std::vector<cv::Point3d>> worldsquarecoordinates;
	//Every square size taking 3x3 square size
	worldsquarecoordinates[0].push_back(cv::Point3d(0, 0, 0));
	worldsquarecoordinates[0].push_back(cv::Point3d(30, 0, 0));
	worldsquarecoordinates[0].push_back(cv::Point3d(30, 30, 0));
	worldsquarecoordinates[0].push_back(cv::Point3d(0, 30, 0));
	//2nd square
	worldsquarecoordinates[0].push_back(cv::Point3d(0, 0, 0));
	worldsquarecoordinates[0].push_back(cv::Point3d(30, 0, 0));
	worldsquarecoordinates[0].push_back(cv::Point3d(30, 30, 0));
	worldsquarecoordinates[0].push_back(cv::Point3d(0, 30, 0));
	//3rd square
	worldsquarecoordinates[0].push_back(cv::Point3d(0, 0, 0));
	worldsquarecoordinates[0].push_back(cv::Point3d(30, 0, 0));
	worldsquarecoordinates[0].push_back(cv::Point3d(30, 30, 0));
	worldsquarecoordinates[0].push_back(cv::Point3d(0, 30, 0));

	std::vector<std::vector<cv::Point2d>> squarecoordinates;
	//1st square
	squarecoordinates[0].push_back(cv::Point2d(457, 127));
	squarecoordinates[0].push_back(cv::Point2d(473, 322));
	squarecoordinates[0].push_back(cv::Point2d(668, 346));
	squarecoordinates[0].push_back(cv::Point2d(674, 136));
	//2nd square
	squarecoordinates[0].push_back(cv::Point2d(452, 417));
	squarecoordinates[0].push_back(cv::Point2d(351, 542));
	squarecoordinates[0].push_back(cv::Point2d(592, 592));
	squarecoordinates[0].push_back(cv::Point2d(652, 447));
	//3rd square
	squarecoordinates[0].push_back(cv::Point2d(212, 217));
	squarecoordinates[0].push_back(cv::Point2d(264, 457));
	squarecoordinates[0].push_back(cv::Point2d(383, 346));
	squarecoordinates[0].push_back(cv::Point2d(356, 143));

	cv::Mat k = cv::initCameraMatrix2D(worldsquarecoordinates, squarecoordinates, cv::Size(1037, 687));
	cv::Mat D;
	cv::Mat R;
	cv::Mat t;
	cv::calibrateCamera(worldsquarecoordinates,squarecoordinates,cv::Size(687,1037),k,D,R,t);




	return 0;
}