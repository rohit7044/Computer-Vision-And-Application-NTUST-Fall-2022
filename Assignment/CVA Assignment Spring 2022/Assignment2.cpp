// For File reading and Data manipulation including matrix multiplication
#include <iostream>
// For image manipulation
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

cv::Mat homography(std::vector<cv::Point> pts_src, std::vector<cv::Point> pts_dst, cv::Mat baseimage, cv::Mat result, cv::Mat mask) {
	//Using predefined findHomography and warpPerspective to project the image
	//After that using bitwise operation to swap the position of first frame and second frame
	cv::Mat h = findHomography(pts_src, pts_dst);
	cv::Mat im_out(540, 760, baseimage.type(), cv::Scalar(0, 0, 0));
	cv::warpPerspective(result, im_out, h, result.size());
	cv::Mat warpedimage(540, 760, baseimage.type(), cv::Scalar(0, 0, 0));
	cv::bitwise_or(im_out, im_out, im_out);
	cv::bitwise_not(mask, mask);
	cv::bitwise_and(baseimage, mask, warpedimage);
	cv::bitwise_or(warpedimage, im_out, warpedimage);
	return warpedimage;
}
cv::Mat maskcreator(cv::Mat& original, std::vector< std::vector<cv::Point> > co_ordinates, cv::Mat& mask) {
	// Using drawContours and bitwise operation to generate the mask and extract the frame
	cv::Mat black(original.rows, original.cols, original.type(), cv::Scalar::all(0));
	cv::drawContours(mask, co_ordinates, 0, cv::Scalar(255, 255, 255), cv::FILLED, 8);
	cv::Mat result(original.rows, original.cols, original.type(), cv::Scalar(0, 0, 0));
	cv::bitwise_and(original, mask, result);
	return result;
}
int main(void) {
	// Read the Image File
	std::cout << "!!!!!PLEASE MAKE SURE TO REMOVE THE QUOTATIONS WHILE ENTERING THE FILE PATH!!!!!" << std::endl;
	std::cout << "Enter the Image filepath inorder to find the homography of the two frames" << std::endl;
	std::string filepath;
	std::getline(std::cin, filepath);
	cv::Mat original = cv::imread(filepath,1);
	original.convertTo(original, CV_8UC1);
	//Get the Co-ordinate of the first frame
	std::vector< std::vector<cv::Point> >  firstFrameco_ordinates;
	firstFrameco_ordinates.push_back(std::vector<cv::Point>());
	firstFrameco_ordinates[0].push_back(cv::Point(24, 108));
	firstFrameco_ordinates[0].push_back(cv::Point(25, 444));
	firstFrameco_ordinates[0].push_back(cv::Point(194, 369));
	firstFrameco_ordinates[0].push_back(cv::Point(198, 157));
	// Create the second mask and extract the first frame 
	cv::Mat firstframe(original.rows, original.cols, original.type(), cv::Scalar(0, 0, 0));
	cv::Mat firstframemask(540, 760, original.type(), cv::Scalar(0, 0, 0));
	firstframe = maskcreator(original, firstFrameco_ordinates, firstframemask);
	//Get the Co-ordinate of the second frame
	std::vector< std::vector<cv::Point> >  secondFrameco_ordinatestwo;
	secondFrameco_ordinatestwo.push_back(std::vector<cv::Point>());
	secondFrameco_ordinatestwo[0].push_back(cv::Point(377, 178));
	secondFrameco_ordinatestwo[0].push_back(cv::Point(377, 340));
	secondFrameco_ordinatestwo[0].push_back(cv::Point(584, 339));
	secondFrameco_ordinatestwo[0].push_back(cv::Point(584, 176));
	// Create the second mask and extract the second frame
	cv::Mat secondframe(original.rows, original.cols, original.type(), cv::Scalar(0, 0, 0));
	cv::Mat secondframemask(540, 760, original.type(), cv::Scalar(0, 0, 0));
	secondframe = maskcreator(original, secondFrameco_ordinatestwo, secondframemask);

	//Till now we have separated the two photoframes as separate images.
	//Now we calculate the homography.
	// We use the co-ordinates closer to the initial frame co-ordinates for better projection accuracy.

	// Get co=ordinate points from first frame
	std::vector<cv::Point> pts_src;
	pts_src.push_back(cv::Point(25, 110));
	pts_src.push_back(cv::Point(26, 445));
	pts_src.push_back(cv::Point(195, 370));
	pts_src.push_back(cv::Point(199, 165));
	// Get co=ordinate points from second frame
	std::vector<cv::Point> pts_dst;
	pts_dst.push_back(cv::Point(380, 180));
	pts_dst.push_back(cv::Point(380, 341));
	pts_dst.push_back(cv::Point(585, 340));
	pts_dst.push_back(cv::Point(585, 177));
	// Find the homography
	cv::Mat final_image(original.rows, original.cols, original.type(), cv::Scalar(0, 0, 0));
	//Homography from First Image to Second Image
	final_image = homography(pts_src,pts_dst,original,firstframe, secondframemask);
	//Homography from Second Image to First image
	final_image = homography(pts_dst,pts_src,final_image,secondframe, firstframemask);	
	cv::imwrite("ID.jpg", final_image);

}
