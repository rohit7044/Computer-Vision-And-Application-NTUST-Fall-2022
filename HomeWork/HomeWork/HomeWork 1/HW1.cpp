#include <iostream>
#include <fstream>


int main (){
	std::string KData[3];
	std::string RTData[3];
	std::string parameter_filepath = "D:\\College Works\\Spring 2022\\Computer Vision and Applications\\HomeWork\\HomeWork\\HomeWork 1\\Test1-2.txt";
	std::string xyz_filepath = "D:\\College Works\\Spring 2022\\Computer Vision and Applications\\HomeWork\\HomeWork\\HomeWork 1\\Test1.xyz";
	// Read the files and convert to xyz file
	std::ifstream parameter_file(parameter_filepath);	
	float K[3][3];
	float RT[3][4];
	std:: string first_string;
	std:: string second_string;
	/*The first loop will extract the Intrinsic parameter matrix 
	while the second loop will extract the Extrinsic parameter matrix*/
	while (parameter_file >> first_string) {
		//Intrinsic Parameter Loop
		for(int i=0;i<3;i++){
			for(int j =0; j<3;j++){
				float temp = 0.0;
				parameter_file >> temp;
				K[i][j] = temp;
			}
		}
		// Extrinsic Parameter Loop
		while (parameter_file >> second_string) {
			for(int i=0;i<3;i++){
				for(int j =0; j<4;j++){
					float temp = 0.0;
					parameter_file >> temp;
					RT[i][j] = temp;
				}
			}
		}
	}
	parameter_file.close();
	// Read the xyz file
	float X3DData[192][4];
	float x_cordinate,y_cordinate,z_cordinate;
	std::ifstream xyz_file(xyz_filepath);
	int i = 0;
	while (xyz_file >>x_cordinate>>y_cordinate>>z_cordinate) {
		X3DData[i][0] = x_cordinate;
		X3DData[i][1] = y_cordinate;
		X3DData[i][2] = z_cordinate;
		X3DData[i][3] = 1.0;
		i++;

	}
	xyz_file.close();
	// Applying the formula and creating a homogenous point 
	// 3D point in Eucledian Space values (3x1 matrix) which contains x,y,z values
	float xData [192][3];
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

    for (int i = 0; i < 192; i++) {
        for (int j = 0; j < 3; j++) {
        	xData[i][j] = 0;
        	for (int k = 0; k <4 ; k++) {;
        		xData[i][j] += X3DData[i][k] * KRT[j][k];
        	}
        }
    }

    // Normalizing the values of x to produce the 2d points
    float x_norm[192][3];
    for(int iteration = 0; iteration<192;iteration++){
	    for(int item = 0;item < 3;item++){
	    	x_norm[iteration][item] = xData[iteration][item] / xData[iteration][2];
	    }

    }
    std::cout<<x_norm[0][0]<<std::endl;
    std::cout<<x_norm[0][1]<<std::endl;
    std::cout<<x_norm[0][2]<<std::endl;

 return 0;

}
