// For File reading and Data manipulation including matrix multiplication
#include <iostream>
#include <math.h>
// For image manipulation
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

cv::Mat FundamentalMatrix_Eight_Point_normalized(std::vector<cv::Point2f>& points1, std::vector<cv::Point2f>& points2)
{
	// Traditional 8 point normalized Algorithm /*NOT USED*/

	// Building A Matrix
	cv::Mat A = cv::Mat::zeros(8, 9, CV_32F);
	double u_prime, v_prime, u, v;

	for (int i = 0; i < 8; i++)
	{
		u_prime = points1[i].x;
		v_prime = points1[i].y;
		u = points2[i].x;
		v = points2[i].y;
		A.at<float>(i, 0) = u_prime * u;
		A.at<float>(i, 1) = u_prime * v;
		A.at<float>(i, 2) = u_prime;
		A.at<float>(i, 3) = v_prime * u;
		A.at<float>(i, 4) = v_prime * v;
		A.at<float>(i, 5) = v_prime;
		A.at<float>(i, 6) = u;
		A.at<float>(i, 7) = v;
		A.at<float>(i, 8) = 1;
	}


	// Compute SVD of A Matrix
	cv::Mat U, VT, S;
	cv::SVD::compute(A.clone(), S, U, VT, 4);
	cv::Mat V = VT.t();		//Transpose of VT
	cv::Mat fundamentalMatrix;

	// The entries of F are the components of the column of V corresponding to the least s.v
	cv::Mat F_vec = V.col(V.cols - 1);

	cv::Mat tempF = cv::Mat::zeros(3, 3, CV_32F);
	tempF.at<float>(0, 0) = F_vec.at<float>(0, 0);
	tempF.at<float>(0, 1) = F_vec.at<float>(1, 0);
	tempF.at<float>(0, 2) = F_vec.at<float>(2, 0);
	tempF.at<float>(1, 0) = F_vec.at<float>(3, 0);
	tempF.at<float>(1, 1) = F_vec.at<float>(4, 0);
	tempF.at<float>(1, 2) = F_vec.at<float>(5, 0);
	tempF.at<float>(2, 0) = F_vec.at<float>(6, 0);
	tempF.at<float>(2, 1) = F_vec.at<float>(7, 0);
	tempF.at<float>(2, 2) = F_vec.at<float>(8, 0);


	// To enforce rank 2 constraint

	cv::Mat F_U, F_VT, F_S;
	cv::SVD::compute(tempF.clone(), F_S, F_U, F_VT);

	F_S.at<float>(F_S.rows - 1, 0) = 0;				// Force rank of F to 2

	cv::Mat F_diag = cv::Mat::zeros(3, 3, CV_32F);

	F_diag.at<float>(0, 0) = F_S.at<float>(0, 0);
	F_diag.at<float>(1, 1) = F_S.at<float>(1, 0);
	F_diag.at<float>(2, 2) = F_S.at<float>(2, 0);


	fundamentalMatrix = F_U * F_diag * F_VT;

	return fundamentalMatrix;
}

int computeRank(const cv::Mat1d& input)
{
	// Compute rank of a matrix
	cv::Mat1d w, u, vT;

	// Compute SVD
	cv::SVD::compute(input, w, u, vT);


	cv::Mat1b nonZeroSingularValues = w > 0.0001;

	int rank = cv::countNonZero(nonZeroSingularValues);

	return rank;	// input matrix rank
}

double distance_to_Line(cv::Point line_start, cv::Point line_end, cv::Point point)
{
	//using line equation to find the shortest distance from point to the epiline
	//Using pythagoras theorem
	double normalLength = hypot(line_end.x - line_start.x, line_end.y - line_start.y);
	double distance = (double)((point.x - line_start.x) * (line_end.y - line_start.y) - (point.y - line_start.y) * (line_end.x - line_start.x)) / normalLength;
	return distance;
}

cv::Mat drawepilines(std::vector<cv::Vec3f> epilinepoints,cv::Mat& img1, std::vector<cv::Point2f> imagecoordinates) {
	
    //For points in an image of a stereo pair, computes the corresponding epilines in the other image.
	std::cout <<"Projection Error" << std::endl;
	cv::RNG rng(12345);//Generate Random Color
	for (int i = 0; i<epilinepoints.size();i++) {
		cv::Vec3d l = epilinepoints.at(i);
		double a = l.val[0];
		double b = l.val[1];
		double c = l.val[2];
		double x0, y0, x1, y1;
		x0 = 0;
		y0 = (-c - a * x0) / b;
		x1 = img1.cols;
		y1 = (-c - a * x1) / b;
		cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		cv::line(img1, cv::Point(x0,y0), cv::Point(x1,y1), color, 2);
		cv::circle(img1, cv::Point(imagecoordinates[i]), 5, color, cv::FILLED);
		double projection_error = distance_to_Line(cv::Point(x0, y0), cv::Point(x1, y1), cv::Point(imagecoordinates[i]));
		std::cout << projection_error << std::endl;
	}

	return img1;
}

int main() {
	// Read the Image File
	std::cout << "!!!!!PLEASE MAKE SURE TO REMOVE THE QUOTATIONS WHILE ENTERING THE FILE PATH!!!!!" << std::endl;
	std::cout << "Enter the LEFT perspective Image filepath" << std::endl;
	std::string leftimagepath, rightimagepath;
	//leftimagepath = "D:\\College Works\\Spring 2022\\Computer Vision and Applications\\Assignment\\CVA Assignment Spring 2022\\Assignment 3\\L.jpg";
	std::getline(std::cin, leftimagepath);
	std::cout << "Enter the RIGHT perspective Image filepath" << std::endl;
	//rightimagepath = "D:\\College Works\\Spring 2022\\Computer Vision and Applications\\Assignment\\CVA Assignment Spring 2022\\Assignment 3\\R.jpg";
	std::getline(std::cin, rightimagepath);
	cv::Mat leftimage = cv::imread(leftimagepath,1);
	cv::Mat rightimage = cv::imread(rightimagepath,1);
	// Get 8 co-ordinates from left and right images
	std::vector<cv::Point2f> leftimagecoordinates;
	leftimagecoordinates.push_back(cv::Point(94,179));
	leftimagecoordinates.push_back(cv::Point(124,322));
	leftimagecoordinates.push_back(cv::Point(358,194));
	leftimagecoordinates.push_back(cv::Point(314,433));
	leftimagecoordinates.push_back(cv::Point(464,454));
	leftimagecoordinates.push_back(cv::Point(226,292));
	leftimagecoordinates.push_back(cv::Point(233,172));
	leftimagecoordinates.push_back(cv::Point(400,300));
	std::vector<cv::Point2f> rightimagecoordinates;
	rightimagecoordinates.push_back(cv::Point(124,184));
	rightimagecoordinates.push_back(cv::Point(166,356));
	rightimagecoordinates.push_back(cv::Point(337,214));
	rightimagecoordinates.push_back(cv::Point(310,445));
	rightimagecoordinates.push_back(cv::Point(415,466));
	rightimagecoordinates.push_back(cv::Point(214,301));
	rightimagecoordinates.push_back(cv::Point(391,269));
	rightimagecoordinates.push_back(cv::Point(406,370));
	//Generate fundamental matrix
	cv::Mat fund_matrix =	cv::findFundamentalMat(leftimagecoordinates,rightimagecoordinates,cv::FM_RANSAC,3.0,0.99);
	// Traditional 8 point normalized algorithm(DIDN'T USED IT)
	//cv::Mat fund_matrix =	FundamentalMatrix_Eight_Point(leftimagecoordinates,rightimagecoordinates);
	std::cout << "Fundamental Matrix " << std::endl;
	std::cout << fund_matrix << std::endl;
	// Verifying the rank of matrix
	std::cout << "Rank of matrix is:" << computeRank(fund_matrix) << std::endl;
	
	//Computing Epilines of left image with corresponding points using fundamental matrix and drawing it on right image
	std::vector<cv::Vec3f> leftepilinepoints;
	cv::computeCorrespondEpilines(leftimagecoordinates, 1, fund_matrix, leftepilinepoints);
	cv::Mat left_image_epilines = drawepilines(leftepilinepoints,rightimage,leftimagecoordinates);
	// Saving the file
	cv::imwrite("61047086s.jpg",left_image_epilines);
	return 0;
}