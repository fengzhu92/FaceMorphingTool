// Feng Zhu
// M.Eng Project

// This header file defines a Pixel class 
// And defines an Image type based on the Pixel class. 
#ifndef image_hpp
#define image_hpp

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

// Each pixel of a color image is represented by its red, green and blue values (RGB).
class Pixel {
public:
	Pixel() : r_(0.0f), g_(0.0f), b_(0.0f) {}
	Pixel(float r, float g, float b) : r_(r), g_(g), b_(b) {}
	Pixel(const Pixel&) = default;
	Pixel& operator=(const Pixel&) = default;
	~Pixel() = default;
	const float& r() const {return r_;}
	const float& g() const {return g_;}
	const float& b() const {return b_;}
	float& r() {return r_;}
	float& g() {return g_;}
	float& b() {return b_;}
private:
	float r_;
	float g_;
	float b_;
};


// Define an Image type as a two-dimensional Pixel array. 
// Each element in the array contains the corresponding RGB values. 
using Image = SPL::Array2<Pixel>;


// Operator overloading
// multiply a pixel with an integer
Pixel operator*(float a, const Pixel& p);

// divide a pixel with an integer
Pixel operator/(const Pixel& p, int a);

// add two pixels
Pixel operator+(const Pixel& u, const Pixel& v);

// Read and write images.
bool inputImage(std::istream& in, Image& image, int& w, int& h);


bool loadImage(const std::string& fileName, Image& image, int& w, int& h);

// draw image
void drawImage(Image& image, float bright);

// save image
bool saveImage(const Image& image, std::string& fileName); 


#endif
