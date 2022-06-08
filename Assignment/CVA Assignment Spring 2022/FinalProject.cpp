// For File reading and Data manipulation including matrix multiplication
#include <iostream>
#include <fstream>
// For image manipulation
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

int main() {
	// Problem 1: 3D Reconstruct Data
	// Predefined Elements
	// Intrinsic Parameter of Left Camera
	cv::Mat LK = (cv::Mat_<double>(3, 3) <<
		1035.278669095568, 0.000000000000, 295.500377771516,
		0.000000000000, 1034.880664685675, 598.224722223280,
		0.000000000000, 0.000000000000, 1.000000000000);
	// Extrinsic Parameter of Left Camera
	cv::Mat LRT = (cv::Mat_<double>(3, 4) <<
		1.000000000000, 0.000000000000, 0.000000000000, 0.000000000000,
		0.000000000000, 1.000000000000, 0.000000000000, 0.000000000000,
		0.000000000000, 0.000000000000, 1.000000000000, 0.000000000000);
	// Left camera Projection Matrix
	cv::Mat LP = LK * LRT;

	// Intrinsic Parameter of Right Camera
	cv::Mat RK = (cv::Mat_<double>(3, 3) <<
		1036.770200759934, 0.000000000000, 403.040387412710,
		0.000000000000, 1037.186415753241, 612.775486819306,
		0.000000000000, 0.000000000000, 1.000000000000);
	// Extrinsic Parameter of Right Camera
	cv::Mat RRT = (cv::Mat_<double>(3, 4) <<
		0.958173249509, 0.009400631103, 0.286034354684, - 69.855978076557,
		- 0.009648701145, 0.999953303246, - 0.000542119475, 0.110435878514,
		- 0.286026094074, - 0.002240415626, 0.958219209809, 14.517584144224);
	// Right camera Projection Matrix
	cv::Mat RP = RK * RRT;
	// Fundamental Matrix
	cv:: Mat F = (cv::Mat_<double>(3, 3) <<
		-0.000000076386, 0.000003333642, - 0.001745446202,
		0.000000105635, - 0.000000103805, - 0.011352133262,
		- 0.000285872783, 0.010010406460, 1.000000000000);
	// Separating some Values for outlining the epipolar Area
	cv::Mat p1, p2, p3, pp1, pp2, pp3;
	p1 = (cv::Mat_<double>(4, 1) <<
		LP.at<double>(0, 0), LP.at<double>(0, 1), LP.at<double>(0, 2), LP.at<double>(0, 3));
	p2 = (cv::Mat_<double>(4, 1) <<
		LP.at<double>(1, 0), LP.at<double>(1, 1), LP.at<double>(1, 2), LP.at<double>(1, 3));
	p3 = (cv::Mat_<double>(4, 1) <<
		LP.at<double>(2, 0), LP.at<double>(2, 1), LP.at<double>(2, 2), LP.at<double>(2, 3));

	pp1 = (cv::Mat_<double>(4, 1) <<
		RP.at<double>(0, 0), RP.at<double>(0, 1), RP.at<double>(0, 2), RP.at<double>(0, 3));
	pp2 = (cv::Mat_<double>(4, 1) <<
		RP.at<double>(1, 0), RP.at<double>(1, 1), RP.at<double>(1, 2), RP.at<double>(1, 3));
	pp3 = (cv::Mat_<double>(4, 1) <<
		RP.at<double>(2, 0), RP.at<double>(2, 1), RP.at<double>(2, 2), RP.at<double>(2, 3));
	// Path of Images
	std::string image_path = "D:\\College Works\\Spring 2022\\Computer Vision and Applications\\Assignment\\CVA Assignment Spring 2022\\Final Project\\FinalProject\\";
	std::string colorimage_path = "D:\\College Works\\Spring 2022\\Computer Vision and Applications\\Assignment\\CVA Assignment Spring 2022\\Final Project\\FinalProject\\TextureImage.jpg";
	// Read Left and Right Image
	std::stringstream Picnamel, Picnamer;
	int pnum = 293;
	cv::Mat *PicL = new cv::Mat[pnum];
	cv::Mat *PicR = new cv::Mat[pnum];
	for (int i = 0; i < pnum; i++) {
		Picnamel.str("");
		Picnamel.clear();
		Picnamer.str("");
		Picnamer.clear();
		if (i < 10) {
			Picnamel << "L\\" << "L00" << i << ".jpg";
			Picnamer << "R\\" << "R00" << i << ".jpg";
		}
		if (i >= 10 && i < 100) {
			Picnamel << "L\\" << "L0" << i << ".jpg";
			Picnamer << "R\\" << "R0" << i << ".jpg";
		}
		if (i >= 100) {
			Picnamel << "L\\" << "L" << i << ".jpg";
			Picnamer << "R\\" << "R" << i << ".jpg";
		}
		PicL[i] = cv::imread(image_path+Picnamel.str(), 0);
		PicR[i] = cv::imread(image_path+Picnamer.str(), 0);
	}
	int rows = PicL[0].rows, cols = PicL[0].cols;
	// Read Texture Image
	cv::Mat TextureImage = cv::imread(colorimage_path);
	 cv::cvtColor(TextureImage, TextureImage, cv::COLOR_BGR2RGB);
	// Create 3D file
	std::fstream outf;
	outf.open("D:\\College Works\\Spring 2022\\Computer Vision and Applications\\Assignment\\CVA Assignment Spring 2022\\Final Project\\FinalProject\\output.xyz", std::ios::out);
	int temp, temp1, maxw = -1, ymax = 0, ymin = 0;
	double temp2;
	cv::Mat lpoint, rpointc, t, rpoint;
	cv::Mat a, b, c, d, A, U, S, vt, V, X;
	// Storing the values in 3d format
	std::vector<std::vector<cv::Point3f> > objectPoints;
	objectPoints.push_back(std::vector<cv::Point3f>());
	// Read each image
	for (int i = 0; i < pnum; i++) {
		for (int h = 0; h < rows; h++) {							
			temp = -1, maxw = -1;
			for (int w = 0; w < cols; w++) {	
				// For Left Image
				if (PicL[i].at<uchar>(h, w) > temp && PicL[i].at<uchar>(h, w) > 150) {	//check for Brightness pixel value greater than 150				
					temp = PicL[i].at<uchar>(h, w);
					maxw = w; // keeping the row index value
				}
			}
			if (maxw != -1) {				//Find the brightest point and perform epipolar line
				lpoint = (cv::Mat_<double>(3, 1) << maxw, h, 1);		
				temp2 = 1;
				t = F * lpoint;	//epipolar line
				temp = -t.at<double>(2, 0) / t.at<double>(1, 0);
				temp1 = -(t.at<double>(0, 0) * cols + t.at<double>(2, 0)) / t.at<double>(1, 0);
				ymax = cv::max(temp, temp1) < rows ? cv::max(temp, temp1) : rows;	//Out of bounds Error Handling
				ymin = cv::min(temp, temp1) < 0 ? 0 : cv::min(temp, temp1);
				// For Right Image
				for (int hh = ymin; hh < ymax; hh++)	//Finding the corresponding point on right
					for (int ww = 0; ww < cols; ww++) {
						if (PicR[i].at<uchar>(hh, ww) > 180) {		//Check for Brightness pixel value greater than 180
							rpointc = (cv::Mat_<double>(3, 1) << ww, hh, 1);
							t = rpointc.t() * F * lpoint;	//Epipolar Line			
							if (abs(t.at<double>(0, 0)) < temp2 && abs(t.at<double>(0, 0)) < 0.10) {		//Taking 0.01 as the shortest distance
								temp2 = abs(t.at<double>(0, 0));
								rpoint = (cv::Mat_<double>(3, 1) << ww, hh, 1);		//Estimating the final point on Right
							}
						}
					}
			}

			if (maxw != -1 && temp2 != 1) {		//Repeat the same for better accuracy
				int u = lpoint.at<double>(0, 0), v = lpoint.at<double>(1, 0);
				int	up = rpoint.at<double>(0, 0), vp = rpoint.at<double>(1, 0);

				a = u * p3.t() - p1.t();
				b = v * p3.t() - p2.t();
				c = up * pp3.t() - pp1.t();
				d = vp * pp3.t() - pp2.t();

				A = (cv::Mat_<double>(4, 4) <<
					a.at<double>(0, 0), a.at<double>(0, 1), a.at<double>(0, 2), a.at<double>(0, 3),
					b.at<double>(0, 0), b.at<double>(0, 1), b.at<double>(0, 2), b.at<double>(0, 3),
					c.at<double>(0, 0), c.at<double>(0, 1), c.at<double>(0, 2), c.at<double>(0, 3),
					d.at<double>(0, 0), d.at<double>(0, 1), d.at<double>(0, 2), d.at<double>(0, 3)
					);

				cv::SVD::compute(A, U, S, vt, cv::SVD::FULL_UV);	//SVD of A	
				V = vt.t();
				X = (cv::Mat_<double>(4, 1) <<
					V.at<double>(0, 3) / V.at<double>(3, 3), V.at<double>(1, 3) / V.at<double>(3, 3), V.at<double>(2, 3) / V.at<double>(3, 3), 1);
				A = A * X;
				temp2 = sqrt(pow(A.at<double>(0, 0), 2) + pow(A.at<double>(1, 0), 2) + pow(A.at<double>(2, 0), 2) + pow(A.at<double>(3, 0), 2));
				if (temp2 < 60)	//removing possible outlier less than 60
					outf << X.at<double>(0, 0) << " " << X.at<double>(1, 0) << " " << X.at<double>(2, 0) << std::endl;	//Write it on XYZ file
				    objectPoints[0].push_back(cv::Point3f(X.at<double>(0, 0), X.at<double>(1, 0), X.at<double>(2, 0))); // keeping the values in an array for further processing
	 
			}
		}
	}
	outf.close();
	// Problem 2 : Finding texture from a 2Dimage and projecting to 3D image
	// Creating a new txt file in the format of XYZRGB
	   std::fstream outftxt;
	   outftxt.open("D:\\College Works\\Spring 2022\\Computer Vision and Applications\\Assignment\\CVA Assignment Spring 2022\\Final Project\\FinalProject\\output.txt", std::ios::out);
	// Creating vector to store vectors of 2D points
		std::vector<std::vector<cv::Point2f> > imgpoints;
	// Defining the world coordinates for 3D points
		std::vector<std::vector<cv::Point3f> > objp;
	// Put the values of 3D point
	// Taking 8 points
		objp.push_back(std::vector<cv::Point3f>());
		objp[0].push_back(cv::Point3f(9.54006, -30.5266, 165.988));
		objp[0].push_back(cv::Point3f(14.5856, -38.278, 176.606));
		objp[0].push_back(cv::Point3f(8.61549, 37.5356, 169.9));
		objp[0].push_back(cv::Point3f(26.1229, -26.235, 169.56));
		objp[0].push_back(cv::Point3f(10.7733, -52.4768, 178.44));
		objp[0].push_back(cv::Point3f(-19.6369, 7.63137, 165.957));
		objp[0].push_back(cv::Point3f(-3.74599, 29.2928, 165.021));
		objp[0].push_back(cv::Point3f(27.276, -6.74583, 169.601));
	// Put the values of 2D point
	// Taking 8 points
		imgpoints.push_back(std::vector<cv::Point2f>());
		imgpoints[0].push_back(cv::Point2f(1272.0, 579.0));
		imgpoints[0].push_back(cv::Point2f(1439,496));
		imgpoints[0].push_back(cv::Point2f(1272,2132));
		imgpoints[0].push_back(cv::Point2f(1705,697));
		imgpoints[0].push_back(cv::Point2f(1346,186));
		imgpoints[0].push_back(cv::Point2f(628,1508));
		imgpoints[0].push_back(cv::Point2f(997,1950));
		imgpoints[0].push_back(cv::Point2f(1695,1071));
	// Define the intrinsic and extrinsic parameters
		cv::Mat cameraMatrix = (cv::Mat_<float>(3, 3)<<
			1,0,0,
			0,1,0,
			0,0,1);
		cv::Mat distCoeffs, R, T;
		cv::calibrateCamera(objp, imgpoints, cv::Size(720, 1280), cameraMatrix, distCoeffs, R, T,cv::CALIB_USE_INTRINSIC_GUESS);
	// Get 2d co-ordinate from 3d co-ordinate
		cv::cvtColor(TextureImage, TextureImage, cv::COLOR_BGR2RGB);
		std::vector<cv::Point2f> imgpointsfinal;
		cv::projectPoints(objectPoints[0], R, T, cameraMatrix, distCoeffs, imgpointsfinal);
	// Write it on XYZRRGB format on a txt file
		for (int i = 0; i <imgpointsfinal.size();i++ ){
			outftxt << objectPoints[0][i].x << " " 
			<< objectPoints[0][i].y << " " 
			<< objectPoints[0][i].z << " " 
			<<int(TextureImage.at<cv::Vec3b>(int(imgpointsfinal[i].x), int(imgpointsfinal[i].y))[0])<< " "
			<<int (TextureImage.at<cv::Vec3b>(int(imgpointsfinal[i].x), int(imgpointsfinal[i].y))[1])<< " " 
			<<int (TextureImage.at<cv::Vec3b>(int(imgpointsfinal[i].x), int(imgpointsfinal[i].y))[2])<< std::endl;
		}
		
	return 0;
}