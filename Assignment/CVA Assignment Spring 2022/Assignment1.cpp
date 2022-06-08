// For File reading and Data manipulation including matrix multiplication
#include <iostream>
#include <fstream>
// For image manipulation
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

int main()
{
	// Copy the paramenter file path and the trajectory file path in the respected variable below
	std::string KData[3];
	std::string RTData[3];
	std::string parameter_filepath,xyz_filepath;
	std::cout << "!!!!!PLEASE MAKE SURE TO REMOVE THE QUOTATIONS WHILE ENTERING THE FILE PATH!!!!!" << std::endl;
	std::cout << "Enter the camera parameter filepath and 3D Coordinate filepath" << std::endl;
	std::getline(std::cin, parameter_filepath);
	std::cout << "Enter the 3D Coordinate filepath" << std::endl;
	std::getline(std::cin, xyz_filepath);
	// Read the files and convert to xyz file
	std::ifstream parameter_file(parameter_filepath);
	float K[3][3];
	float RT[3][4];
	std::string first_string;
	std::string second_string;
	/*The first loop will extract the Intrinsic parameter matrix
	while the second loop will extract the Extrinsic parameter matrix*/
	while (parameter_file >> first_string) {
		//Intrinsic Parameter Loop
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				float temp = 0.0;
				parameter_file >> temp;
				K[i][j] = temp;
			}
		}
		// Extrinsic Parameter Loop
		while (parameter_file >> second_string) {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 4; j++) {
					float temp = 0.0;
					parameter_file >> temp;
					RT[i][j] = temp;
				}
			}
		}
	}
	parameter_file.close();
	// On the condition that the trajectory file has 192 lines so that the matrix can be created beforehand
	// Read the xyz file
	float X3DData[192][4];
	float x_cordinate, y_cordinate, z_cordinate;
	std::ifstream xyz_file(xyz_filepath);
	int i = 0;
	while (xyz_file >> x_cordinate >> y_cordinate >> z_cordinate) {
		X3DData[i][0] = x_cordinate;
		X3DData[i][1] = y_cordinate;
		X3DData[i][2] = z_cordinate;
		X3DData[i][3] = 1.0;// manually adding 1.0
		i++;

	}
	xyz_file.close();
	// Applying the formula and creating a homogenous point 
	// x = K|RT|X
	// 3D point in Eucledian Space values (3x1 matrix) which contains x,y,z values
	float xData[192][3];
	float KRT[3][4];
	// Multiply K and RT
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			KRT[i][j] = 0;
			for (int k = 0; k < 3; k++) {
				KRT[i][j] += K[i][k] * RT[k][j];
			}
		}
	}
	// Multiply KRT with X
	for (int i = 0; i < 192; i++) {
		for (int j = 0; j < 3; j++) {
			xData[i][j] = 0;
			for (int k = 0; k < 4; k++) {
				;
				xData[i][j] += X3DData[i][k] * KRT[j][k];
			}
		}
	}

	// Normalizing the values of x to produce the 2d points
	float x_norm[192][3];
	for (int iteration = 0; iteration < 192; iteration++) {
		for (int item = 0; item < 3; item++) {
			x_norm[iteration][item] = xData[iteration][item] / xData[iteration][2];
		}

	}
	std::cout << "Multiplication Done" << std::endl;
	// Image manipulation
	std::string image_path;
	std::cout << "Enter the image to find the coordinate " << std::endl;
	std::getline(std::cin, image_path);
	cv::Mat original_image = cv::imread(image_path);
	//Creating the point variable and passing it to the image
	cv::Point coordinates;
	for (int i = 0; i < 192;i++) {
		coordinates.x = x_norm[i][0];
		coordinates.y = x_norm[i][1];
		cv::circle(original_image, coordinates, 0, (0, 0, 0), 2);
	}
	std::string final_image_path = image_path;
	final_image_path.resize(image_path.size()-3);
	final_image_path.append("png");
	std::cout << "Created a png file with same name in the same directory." << std::endl;
	cv::imwrite(final_image_path, original_image);
	cv::waitKey(0);


	return 0;
}
