// Feng Zhu
// M.Eng Project

// This header file defines a Pixel class 
// And defines an Image type based on the Pixel class. 

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
#include <cctype>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <unistd.h>
#include <sys/stat.h>
#include "image.hpp"


// Operator overloading
// multiply
Pixel operator*(float a, const Pixel& p)
{
	return Pixel(p.r() * a, p.g() * a, p.b() * a);
}

// divide
Pixel operator/(const Pixel& p, int a)
{
	return Pixel(p.r() / a, p.g() / a, p.b() / a);
}

// add
Pixel operator+(const Pixel& u, const Pixel& v)
{
	return Pixel(u.r() + v.r(), u.g() + v.g(), u.b() + v.b());
}

// Read and write images.
bool loadImage(const std::string& fileName, Image& image, int& w, int& h)
{
	std::ifstream in(fileName.c_str());
	
	// read the PPM image using SPL::decodePpm
	SPL::Array2<int> red;
	SPL::Array2<int> green;
	SPL::Array2<int> blue;
	int maxVal;
	bool sgnd;
	if(SPL::decodePpm(in, red, green, blue, maxVal, sgnd)) {
		std::cerr << "Error! Failed to load image: "<<fileName<<"\n";
		exit(1);
	}
	
	// get the width and height
	w = red.getWidth();
	h = red.getHeight();

	// allocate memory for the image
	image = Image(w, h, Pixel(0,0,0));
	
	// assign pixel values to image
	for(int i = 0; i<w; ++i) {
		for(int j = 0; j<h; ++j) {
			image(i,j).r() = red(i,j);
			image(i,j).g() = green(i,j);
			image(i,j).b() = blue(i,j);
		
		}
	} 
	// on success return true
	return true;
}

// save image to ppm file
bool saveImage(const Image& image, std::string& fileName) 
{
	int w = image.getWidth();
	int h = image.getHeight();
	int maxVal;
	bool sgnd;
	SPL::Array2<int> red(w,h);
	SPL::Array2<int> green(w,h);
	SPL::Array2<int> blue(w,h);
	
	for(int i = 0; i<w; ++i) {
		for(int j = 0; j<h; ++j) {
			red(i,j) = image(i,j).r();
			green(i,j) = image(i,j).g();
			blue(i,j) = image(i,j).b();
		}
	} 
	// open the data file
	std::ofstream ofs(fileName.c_str());
	if(ofs.is_open()) {
		if(SPL::encodePpm(ofs, red, green, blue, 255, false, false)) {
			std::cerr << "Onput error! Failed to write the image to PPM file!"<<"\n";
			return false;
		}
		else {
			return true;
		}
	}
	else {
		std::cerr<<"Error! Failed to open file: "<<fileName<<"\n";
		exit(1);
	}
}

void drawImage(Image& image, float bright)
{
	Image imageToChange = image;
	for (int x = 0; x < image.getWidth(); ++x) {
		for(int y = 0; y < image.getHeight(); ++y) {
			imageToChange(x,y) = (bright * imageToChange(x, y))/255;
		}
	}
	glDrawPixels(image.getWidth(), image.getHeight(), GL_RGB, GL_FLOAT, &imageToChange(0,0));
}

















