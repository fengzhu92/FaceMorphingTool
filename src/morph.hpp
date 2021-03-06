// Feng Zhu
// M.Eng Project

// This header file declares some functions to warp and morph images.
#ifndef morph_hpp
#define morph_hpp

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
#include "image.hpp"
#include "feature.hpp"



/////////////////////////////////////////////////////////////////////////////
// Code for interpolating 
////////////////////////////////////////////////////////////////////////////////

// linear interpolation
float interpolate(float c1, float c2, float a);


// compute color
Pixel colorInter(Pixel p1, Pixel p2, float t);

// pixel interpolate to an image
Pixel lerp(Point& X_p, const Image& image);

// point edge detection
Point edge(Point& p, const Image& image);

////////////////////////////////////////////////////////////////////////////////
// Code for blending two images
////////////////////////////////////////////////////////////////////////////////

void blend(Image& blendImg, const Image& first, const Image& second, float t);


// compute a warped image from a source image with two corresponding feature line segments 
// can be used to warp in both directions
// a,b and p are parameters for calculating the weight of each line segment in the feature set
// the output is a warped image of type Image
bool warpImage(Image& warpedImage, const Image& sourceImage, const FeatureSet& sourceFeatureSet, const FeatureSet& targetFeatureSet, float a, float b, float p, float t);
////////////////////////////////////////////////////////////////////////////////
// Code for morph two images
////////////////////////////////////////////////////////////////////////////////
/* compute a morphed image from two images by warping each towards the intermediate frame and then blending them
// d is the paramenter for cross-dissolving proceess
// the output is a morphed image
*/

void morphImage(Image& resultImg, const Image& sourceImage, const FeatureSet& sourceFeatureSet, const Image& targetImage, const FeatureSet& targetFeatureSet, float a, float b, float p, float t);

///////////////////////////////////////////////////////////////////////////////
// Code for generating frames.
////////////////////////////////////////////////////////////////////////////////

// compute the digits of a number
int countDigits(int number);

// generate file names with a given basename
bool makeFileName(std::string& fileName, std::string& basename, int index, int digits);



#endif
