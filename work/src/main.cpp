// std
#include <iostream>

// opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>

// project
#include "invert.hpp"

cv::Vec3b getPixel(int row, int col, int t, int s);

using namespace cv;
using namespace std;

// image array is 17x17 images of size 1024x512
Mat image_array[17][17];

Point uvCoord[17][17];

// main program
// 
int main( int argc, char** argv ) {

	std::cout << "Loading light field ..." << std::endl;
	std::vector < cv::String > lf_imgs;
	cv::glob(argv[1], lf_imgs);
	for (cv::String cv_str: lf_imgs) {
	    // get the filepath
	    std::string filepath(cv_str);
	    size_t pos = filepath.find_last_of("/\\");
	    if (pos != std::string::npos) {
	        // replace "_" with " "
	        std::string filename = filepath.substr(pos + 1);
	        pos = 0;
	        while ((pos = filename.find("_", pos)) != std::string::npos) {
	            filename.replace(pos, 1, " ");
	            pos++;
	        }
	        // parse for values
	        std::istringstream ss(filename);
	        std::string name;
	        int row, col;
	        float v, u;
	        ss >> name >> row >> col >> v >> u;
	        if (ss.good()) {
	            // TODO something with the image file "filepath"
	            // TODO something with the coordinates: row, col, v, u
	        	image_array[row][col] = imread(filepath, CV_LOAD_IMAGE_COLOR);

	        	uvCoord[row][col] = Point(u,v);

	            continue;
	        }
	    }
    // throw error otherwise
    	std::cerr << "Filepath error with : " << filepath << std::endl;
    	std::cerr << "Expected in the form : [prefix]/[name]_[row]_[col]_[v]_[u][suffix]";
    	abort();
	}
	std::cout << "Finished loading light field" << std::endl;

	// change focal to change the focus of image eg. 0.7, 0.8
	double focal = 1;
	double cu = 533.057190;
	double cv = -776.880371;
	Mat stImage(512, 1024, CV_8UC3);

	// create the image with a new focal length
	for(int i = 0; i < 512; i++){
		for(int j = 0; j < 1024; j++){
			int count = 0;
			Vec3i vec(0,0,0);

			for(int row = 0; row < 17; row++){
				for(int col = 0; col < 17; col++){

					double du = cu - uvCoord[row][col].x;
					double dv = cv - uvCoord[row][col].y;

					int s = floor(du+((focal*j-du)/focal));
					int t = floor(dv+((focal*i-dv)/focal));
					
					if(t < 0 || t > 511 || s < 0 || s > 1023){
						continue;
					}

					vec += getPixel(row, col, t, s);
					count++;
				}
			}
			stImage.at<Vec3b>(i, j) = vec/count;
		}
	}

	// create a window for display and show our image inside it
	string img_display = "Focal Image Display";
	namedWindow(img_display, WINDOW_AUTOSIZE);
	imshow(img_display, stImage);


	// wait for a keystroke in the window before exiting
	waitKey(0);
}


// find out the value of the pixel of the image in image_array[row][col]
// and at position (t,s)
cv::Vec3b getPixel(int row, int col, int t, int s){
	Vec3b value = image_array[row][col].at<Vec3b>(t,s);
	return value;
}

