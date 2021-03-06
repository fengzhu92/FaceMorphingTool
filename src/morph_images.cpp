// Feng Zhu
// Univerisity of Victoria
// M.Eng Project

/*
    This is the morphing program to generate intermediate frames fram source and target images.
    The inputs are source and target images, two feature line segment DAT files, and several parameters
    including number of frames, direcotry of where the frames go, and flags to indicate display on or off.

    This program should run after the GUI program because it takes the outputs of GUI program.
    The source and target images in GUI and this program should be idential.
*/

#include <SPL/Array2.hpp>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Point_2.h>
#include <CGAL/Segment_2.h>
#include <CGAL/Vector_2.h>
#include <SPL/cgalUtil.hpp>
#include <math.h>
#include <GL/glut.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cctype>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sys/stat.h>
#include "image.hpp"
#include "feature.hpp"
#include "morph.hpp"

////////////////////////////////////////////////////////////////////////////////
// Global data type.
////////////////////////////////////////////////////////////////////////////////
struct Global
{
	Image currentImg;
	Image morphImg;
	Image drawImg;
	Point startPos;
	Point currentPos;
	FeatureSet currentFeature;
	std::vector<std::string> fileNames;
	std::vector<Image> images;
        std::vector<Image> frames;
	std::vector<std::string> featureNames;
	std::vector<FeatureSet> featureSets;
	std::string basename;		// used for generating frame names
	std::string direc = "NULL";	// initialize the path of frame folder
	int frameNum;			// the number of frames to be generated
        int fr = 0;			// the index of current frame
	int num;			// the number of images read
	int digits;			// the digit of frame filename
	int digits_flag = 0; 		// a flag indicates if digit is provided
	int imageX;			// the Width of image
	int imageY;			// the height of image
	int saveFlag = 0;		// if is 1, frames will be saved
	int displayFlag = 0;		// if is 1, animation will be displayed
	int displayTime = 0;
	int framePeriod = 300;
	float a = 1.0f;			// a default value for parameter a	
	float b = 1.0f;			// a default value for parameter b
	float p = 0.5f;			// a default value for parameter p

};


////////////////////////////////////////////////////////////////////////////////
// Global data.
////////////////////////////////////////////////////////////////////////////////

Global global;

//////////////////////////////////////////////////////////////////////////////////////
//  Usage Information
//////////////////////////////////////////////////////////////////////////////////////

// handle invalid command line parameters
// specify the command line interface
void usage()
{
	std::cerr
		<< "Usage:\n"
		<< "morph_images [-i sourceImage] [-f sourceFeatureFile] [-i tagetImage] [-f targetFeatureFile] [-n frameNum] [-b baseName] [-s] [-g] [-t] \n"
		<< "[-i soruceImage]: the source image (PPM) to be warped\n"
		<< "[-f sourceFeatureFile]: the feature set data file for source image\n"
		<< "[-i targetImage]: the target image (PPM) to be morphed to\n"
		<< "[-f targetFeatureFile]: the feature set data file for target image\n"
		<< "[-n frameNum]: the number of frames to be generated\n"
		<< "[-b baseName]: the base name of frames\n"
		<< "[-s]: optional. specify whether to save frames. turned on by default\n"
		<< "[-g]: optional. specify whether to display the animation. turned by by default\n"
		<< "[-t]: optional. specify whether to print out the run time of morphing. turned off by default\n"
		<< "[-m]: optional. specify a parameter in transformation\n"
		<< "[-n]: optional. specify a parameter in transformation\n"
		<< "[-p]: optional. specify a parameter in transformation\n"
		;
	exit(1);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// generate frames to files with name given
//////////////////////////////////////////////////////////////////////////////////////////////
void generateFrames(const Image& sourceImage, const FeatureSet& sourceFeatureSet, const Image& targetImage, const FeatureSet& targetFeatureSet, float a, float b, float p, int turn)
{
	// t is the parameter used for morphing
	// t is started with 0
	float t = 0;

	// check if images should be pushed back to vector
	if (global.displayFlag == 1) {
		// calculate the memory needed in Gega Bytes
		double memory = 3 * 4 * global.imageX * global.imageY * global.frameNum/double(1024)/1024/1024;
		// check if the memeory required exceeds 1GB 
		if (memory < 1) {
			global.displayFlag = 1;
		}
		else {
			std::cout<<"Too much memory (>1GB) required: "<<memory<<" GB\n"
				<<"Please enable '-g' option or use smaller images.\n";
			// Turn off '-g' option. No animation.
			global.displayFlag = 0;
		}
	}
	
	for(int i = 0; i<global.frameNum; ++i) {
		std::cout << "Morphing frame #" << i + global.frameNum * turn << "...\n";
		float ease_t = powf(t, 2.f)*(3 - 2*t);
		Image result;
		std::string frameName;
		int frame_digits = countDigits((global.images.size() - 1)*global.frameNum);
		if (global.digits_flag == 1) {
			if(global.digits >= frame_digits) {
				frame_digits = global.digits;
			}
			else {
				std::cerr<<"Error! Input digit is less than required!\n";
				exit(1);
			}
		}
		// create a frame name of the current frame
		makeFileName(frameName, global.basename, i + global.frameNum * turn, frame_digits);
		// morph conducted here
		morphImage(result, sourceImage, sourceFeatureSet, targetImage, targetFeatureSet, a, b, p, ease_t);
		if(global.displayFlag == 1) {
			global.frames.push_back(result);
		}
		if(global.saveFlag == 1) {
			// save frames to the folder created
			saveImage(result, frameName);
		}
		// increase the t
		t += (1.0/global.frameNum);		              
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GLUT callback functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// display call back function
void displayMorph()
{
	// clear the color buffer 
	glClear(GL_COLOR_BUFFER_BIT);
	// display the current frame	
	drawImage(global.frames[global.fr], 1.0);
	glutSwapBuffers();
}

// keyboard call back function
void keyboard(unsigned char key, int x, int y)
{
	switch(key) {
		case 'q':
			exit(0);
			break;
	}
}

// timer call back function
void timer(int value)
{
	// if the current frame is not the last one
        if (global.fr < global.frames.size()-1) 
	{
		// go the the next frame
		global.fr += 1;
	} 
	// re-draw
	glutPostRedisplay();
	glutTimerFunc(global.framePeriod, timer, 0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main Function
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	// check the validity of command line.
	if (argc < 13) {
		std::cerr << "Error! Too few command line arguments\n";
		usage();
	}
	char *cvalue = NULL;
	int c;
	opterr = 0;
	while((c = getopt(argc, argv, "i:f:n:b:d:a:m:p:gst")) != -1) {
		switch (c) {
			case 'i':
				// get the input option
				cvalue = optarg;
				// push back the image file names
				global.fileNames.push_back(cvalue);
				// load the images 
				loadImage(cvalue, global.currentImg, global.imageX, global.imageY);
				// push back the images to the vector of images
				global.images.push_back(global.currentImg);
				break;
			case 'f':
				// get the input option
				cvalue = optarg;
				global.featureNames.push_back(cvalue);
				// load the feature line segments set
				loadFeatureSet(cvalue, global.currentFeature);
				// puch back the feature set to a vector of feature sets
				global.featureSets.push_back(global.currentFeature); 
				break;
			case 'n':
				// get the input option
				cvalue = optarg;
				global.frameNum = atoi(cvalue);
				break;
			case 'b':
				// get the basename of output frames
				cvalue = optarg;
				global.basename = cvalue;
				break;
			case 'd':
				// how many digits for the basename
				cvalue = optarg;
				global.digits = atoi(cvalue);
				global.digits_flag = 1;
			case 'a':
				// a parameter for image transformation
				cvalue = optarg;
				global.a = atof(cvalue);
				break;
			case 'm':
				// a parameter for image transformation
				cvalue = optarg;
				global.b = atof(cvalue);
				break;
			case 'p':
				// a parameter for image transformation
				cvalue = optarg;
				global.p = atof(cvalue);
				break;
			case 's':
                                // if -g provided, frames will be saved
				global.saveFlag = 1;
				break;
 			case 'g':
				// if -s provided, animation will be displayed
				global.displayFlag = 1;
				break;
			case 't':
				// if -e provided, the running time of morphing will be printed
                                global.displayTime = 1;
				break;
			case '?':
				if(optopt == 'i')
					std::cerr << "Option -"<<(char)optopt <<" requires an argument.\n";
				if(optopt == 'f')
					std::cerr << "Option -"<<(char)optopt <<" requires an argument.\n";
				if(optopt == 'n')
					std::cerr << "Option -"<<(char)optopt <<" requires an argument.\n";
				if(optopt == 'b')
					std::cerr << "Option -"<<(char)optopt <<" requires an argument.\n";
				if(optopt == 'd')
					std::cerr << "Option -"<<(char)optopt <<" requires an argument.\n";
				if(isprint (optopt))
					std::cerr << "Unknow option -"<<(char)optopt<<".\n";
				else
					std::cerr << "Unknown option character "<<(char)optopt<<".\n";
				exit(0);
			default:
				std::cerr<<"error!"<<"\n";
				exit(1);
				
		}
	}

	// the number of images read in
	global.num = global.images.size();
	
	// print out the message
	std::cout<<"the number of images read: "<<global.num<<"\n";

	// check if the the input is correct
	if (global.num < 2) {
		std::cerr << "Too few images! No less than two input images required!\n";
		exit(0);
	}

	// the number of feature sets read in
	int sets = global.featureSets.size();

	// print out the meaasge
	std::cout << "the number of feature sets read: " << sets <<"\n";

	// check if the number of feature sets are the same 
	if (sets != global.num) {
		std::cerr<< "Error! The number of feature sets doesn't match the images read!\n";
		exit(3);
	}

	// check the number of line segments
	for (int kk = 0; kk < global.num - 1; ++kk) {
		if (global.featureSets[kk].size() != global.featureSets[kk+1].size()) {
			std::cerr << "the number of feaure line segments should be the same!\n";
			exit(1);
		} 
	}

	// check the validity of the elements in feature sets
	for (int k = 0; k < global.num; ++k) {
		FeatureSet temp = global.featureSets[k];
		if (temp.size() == 0) 
		{
			std::cerr << "Error: No elements in feature set, please double check feature set files!\n ";
			exit(1);
		}
			for(int kk = 0; kk < temp.size(); ++kk) 
				{
                                	// if the coordinates exceed the image boundary, error message should be printed out.
					if (temp[kk].source().x() > global.imageX || temp[kk].source().y() > global.imageY || 
					temp[kk].target().x() > global.imageX || temp[kk].target().y() > global.imageY) {
						std::cerr << "Error! Feature doesn't match source image! Please double check!\n";
						exit(1);
				}
			}
	}

	// check the validity of the number of output frames
	if(global.frameNum <= 0) {
		std::cerr << "Error!\n"<<"The number of output frames should be larger than 0!\n";
		exit(0);
	}
    
	// start counting the time used for morphing
	clock_t t1, t2;
	t1 = clock();

	// start morphing and ouput intermediate frames
	for (int indx = 0; indx < global.num - 1; ++indx) {
		// start morphing among multiple images
		generateFrames(global.images[indx], global.featureSets[indx], global.images[indx+1], global.featureSets[indx+1], global.a, global.b,global.p, indx);
		t2 = clock();
		if (global.displayTime == 1) {
			float seconds = ((float)t2 - (float)t1)/CLOCKS_PER_SEC;
			std::cout << "time used for morphing: "<< seconds << "s" << "\n";
		}
	}

	// if animation display option is turned on
	if(global.displayFlag == 1) {

		// windowing and rendering
		glutInit(&argc, argv);
		glutInitWindowPosition(600,400);
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
		glutInitWindowSize(global.imageX, global.imageY);
		glutCreateWindow("morphed image");
		glutDisplayFunc(displayMorph);
		glutKeyboardFunc(keyboard);
		glutTimerFunc(global.framePeriod, timer, 0);
	
		glutMainLoop();
	}
	
	return 0;					
}


