// For File reading and Data manipulation including matrix multiplication
#include <iostream>
// For image manipulation
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>


cv::Mat homography(std::vector<cv::Point> pts_src, std::vector<cv::Point> pts_dst, cv::Mat baseimage, cv::Mat anotherimage) {
	//Basic Homography Technique
	cv::Mat h = cv::findHomography(pts_src, pts_dst);
	cv::Mat im_out(500, 500, CV_8UC1, cv::Scalar(0, 0, 0));
	cv::warpPerspective(baseimage, im_out, h, cv::Size(500,500));
	return im_out;
}
std::vector<cv::Point> CornerDetection(cv::Mat baseimage){
	// Finding the 4 corners of a4 sheet as the reference point in less painful way
	cv::Mat gray = baseimage;
	cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);
	std::vector<cv::Point> points;
	cv::goodFeaturesToTrack(gray, points, 4, 0.1, 10);
	return points;
}
void vectorPointtoArrayofArrayCreator(std::vector<cv::Point>& points, std::vector<std::vector<cv::Point>>& sortedcontours) {
	//Vector Points to ArrayofArray creator for use in contour
	sortedcontours.push_back(std::vector<cv::Point>());
	sortedcontours[0].push_back(points[0]);
	sortedcontours[0].push_back(points[2]);
	sortedcontours[0].push_back(points[3]);
	sortedcontours[0].push_back(points[1]);
}
std::vector<cv::Point> sortPoints(std::vector<cv::Point> unsorted) {
	//Sorts a vector of 4 points into top left, top right, bottomleft, bottomright
	std::vector<cv::Point> sorted;
	for (int i = 0; i < 4; i++)sorted.push_back(cv::Point(0, 0));
	int middleX = (unsorted[0].x + unsorted[1].x + unsorted[2].x + unsorted[3].x) / 4;
	int middleY = (unsorted[0].y + unsorted[1].y + unsorted[2].y + unsorted[3].y) / 4;
	for (int i = 0; i < unsorted.size(); i++) {
		if (unsorted.at(i).x < middleX && unsorted.at(i).y < middleY)sorted[0] = unsorted.at(i);
		if (unsorted.at(i).x > middleX && unsorted.at(i).y < middleY)sorted[1] = unsorted.at(i);
		if (unsorted.at(i).x < middleX && unsorted.at(i).y > middleY)sorted[2] = unsorted.at(i);
		if (unsorted.at(i).x > middleX && unsorted.at(i).y > middleY)sorted[3] = unsorted.at(i);
	}
	return sorted;
}
void contourToboundingRect(std::vector<std::vector<cv::Point>> points, int& maxX, int& maxY, int& minX, int& minY)
{
	for (int i = 0; i < points.size(); i++)
		for (int j = 0; j < points[i].size(); j++)
		{
			cv::Point p = points[i][j];

			maxX = std::max(maxX, p.x);
			minX = std::min(minX, p.x);

			maxY = std::max(maxY, p.y);
			minY = std::min(minY, p.y);
		}
}
cv::Mat footSizeCalculator(std::vector<cv::Point> cornerpoints, cv::Mat mask,cv::Mat color_blend_img) {
	//Calculating the foot size
	cv::threshold(mask, mask, 0, 255, cv::THRESH_OTSU);
	cv::Mat blend_img (color_blend_img.rows, color_blend_img.cols, color_blend_img.type(), cv::Scalar(0));
	cv::cvtColor(color_blend_img, blend_img, cv::COLOR_BGR2GRAY);
	cv::Mat gray_blend(blend_img.rows, blend_img.cols, blend_img.type(), cv::Scalar(0,0,0));
	cv::bitwise_and(blend_img,blend_img,gray_blend,mask);
	cv::GaussianBlur(gray_blend,gray_blend,cv::Size(3,3),0);
	std::vector<std::vector<cv::Point>> edges;
	cv::findContours(gray_blend, edges, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
	// Calculating the minimum points for creating a bounding rectangle for the feet
	int feetmaxX = 0, feetminX = gray_blend.cols, feetmaxY = 0, feetminY = gray_blend.rows;
	contourToboundingRect(edges, feetmaxX, feetmaxY, feetminX, feetminY);
	// Calculating the minimum points for creating a bounding rectangle for the a4 sheet
	std::vector<std::vector<cv::Point>> sortedcorners;
	vectorPointtoArrayofArrayCreator(cornerpoints,sortedcorners);
	int papermaxX = 0, paperminX = gray_blend.cols, papermaxY = 0, paperminY = gray_blend.rows;
	contourToboundingRect(sortedcorners, papermaxX, papermaxY, paperminX, paperminY);
	cv::Mat final_bounding_box = color_blend_img;
	cv::rectangle(final_bounding_box, cv::Point(feetminX, feetminY), cv::Point(feetmaxX, feetmaxY), cv::Scalar(0, 0, 255));
	cv::rectangle(final_bounding_box, cv::Point(paperminX, paperminY), cv::Point(papermaxX, papermaxY), cv::Scalar(0, 255, 0));
	// Calculate the Pixel Per Metric using Unitary Method
	float pixelunit;
	int a4sheetlength,feetlength, feetwidth;
	cv::Rect2d feetRect,paperRect;
	feetRect = cv::boundingRect(edges[0]);
	paperRect = cv::boundingRect(sortedcorners[0]);
	a4sheetlength = 297;
	pixelunit = 297/paperRect.size().height;
	feetlength = pixelunit * feetRect.size().height;
	feetwidth = pixelunit * feetRect.size().width;
	std::string feet_length = "LENGTH OF FEET = " + std::to_string(feetlength)+" mm";
	std::string feet_width = "WIDTH OF FEET = " + std::to_string(feetwidth)+" mm";
	//Put Text on image
	cv::Point height(5, 370);
	cv::Point width(5, 390);	
	cv::putText(final_bounding_box, feet_length, height, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.6, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
	cv::putText(final_bounding_box, feet_width, width,cv::FONT_HERSHEY_COMPLEX_SMALL, 0.6,cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
	return final_bounding_box;
}
cv::Mat footremover(cv::Mat original_image,std::vector<cv::Point> corners) {
	//Array of Array Creator for contour points transform
	std::vector<std::vector<cv::Point>> sortedcorners;
	vectorPointtoArrayofArrayCreator(corners,sortedcorners);
	cv::Mat mask_image= original_image;	
	cv::cvtColor(mask_image, mask_image, cv::COLOR_BGR2GRAY);
	cv::drawContours(mask_image, sortedcorners, 0, cv::Scalar(255), 2);	
	cv::threshold(mask_image,mask_image,80,255,cv::THRESH_BINARY_INV);
	cv::floodFill(mask_image, cv::Point(0, 0), cv::Scalar(0));
	cv::Mat kernel=cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(9,9));
	cv::morphologyEx(mask_image,mask_image,cv::MORPH_OPEN,kernel);
	return mask_image;

}

int main(){
	// Read the Image File
	std::cout << "!!!!!PLEASE MAKE SURE TO REMOVE THE QUOTATIONS WHILE ENTERING THE FILE PATH!!!!!" << std::endl;
	std::cout << "Enter the LEFT perspective Image filepath" << std::endl;
	std::string leftimagepath,rightimagepath;
	//leftimagepath = "D:\\College Works\\Spring 2022\\Computer Vision and Applications\\Assignment\\CVA Assignment Spring 2022\\Midterm Project\\Feet_picture_left.png";
	//rightimagepath = "D:\\College Works\\Spring 2022\\Computer Vision and Applications\\Assignment\\CVA Assignment Spring 2022\\Midterm Project\\Feet_picture_right.png";
	std::getline(std::cin, leftimagepath);
	std::cout << "Enter the RIGHT perspective Image filepath" << std::endl;
	std::getline(std::cin, rightimagepath);
	cv::Mat original_left_image = cv::imread(leftimagepath, 1);
	cv::Mat original_right_image = cv::imread(rightimagepath, 1);

	// Get co-ordinate points from left image and sort them 
	std::vector<cv::Point> pts_src = CornerDetection(original_left_image);
	std::vector<cv::Point> pts_src_sorted = sortPoints(pts_src);

	// Get co-ordinate points from right image and sort them
	std::vector<cv::Point> pts_dst = CornerDetection(original_right_image);
	std::vector<cv::Point> pts_dst_sorted = sortPoints(pts_dst);

	// Wraping based on Right Image
	cv::Mat warped_left_image(500, 500, CV_8UC1, cv::Scalar(0, 0, 0));
	warped_left_image = homography(pts_src_sorted, pts_dst_sorted, original_left_image,original_right_image);
	// Blend the images
	cv::Mat blended_image;
	cv::addWeighted(warped_left_image, 0.5, original_right_image, 0.5, 0.0, blended_image);
	// Removing the foot and creating the mask 
	cv::Mat foot_mask = footremover(blended_image, pts_dst_sorted);
	cv::Mat final_image = footSizeCalculator(pts_dst_sorted,foot_mask,blended_image);
	//cv::imshow("Final Bounding Box", final_image);
	//cv::waitKey(0);
	cv::imwrite("61047086s.jpg",final_image);

}