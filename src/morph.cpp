// Feng Zhu
// M.Eng Project

// This header file declares some functions to warp and morph images.

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
#include "morph.hpp"



/////////////////////////////////////////////////////////////////////////////
// Code for interpolating 
////////////////////////////////////////////////////////////////////////////////

float interpolate(float c1, float c2, float a) 
{
	return(c1 + a * (c2 - c1));
}


// compute color
Pixel colorInter(Pixel p1, Pixel p2, float t)
{
	float r = interpolate(p1.r(), p2.r(), t);
	float g = interpolate(p1.g(), p2.g(), t);
	float b = interpolate(p1.b(), p2.b(), t);
	Pixel result(r, g, b);
	return result;
}

// return a point that inside a image boundary
Point edge(Point& p, const Image& image)
{
	int w = image.getWidth();
	int h = image.getHeight();

	// case 1
	if(p.x()<0 && p.y()<0) {
		p = Point(0, 0);
	}
	// case 2
	if(p.x()>=0 && p.x()<w && p.y()<0) {
		p = Point(p.x(), 0);
	}
	// case 3
	if(p.x()>=w && p.y()<0) {
		p = Point(w-1, 0);
	}
	// case 4
	if(p.x()>=w && p.y()>=0 && p.y()<h) {
		p = Point(w-1, p.y());
	} 
	// case 5
	if(p.x()>=w && p.y()>=h) {
		p = Point(w-1, h-1);
	}
	// case 6
	if(p.x()>=0 && p.x()<w && p.y()>=h) {
		p = Point(p.x(), h-1);
	}
	// case 7
	if(p.x()<0 && p.y()>=h) {
		p = Point(0, h-1);
	}
	// case 8
	if(p.x()<0 && p.y()>=0 && p.y()<h) {
		p = Point(0, p.y());
	}
	
	return p;
}

// pixel interpolate to an image
Pixel lerp(Point& X_p, const Image& image)
{
	Point v0(floorf(X_p.x()), floorf(X_p.y()));
	Point v1(ceilf(X_p.x()), floorf(X_p.y()));
        Point v2(floorf(X_p.x()), ceilf(X_p.y()));
	Point v3(ceilf(X_p.x()), ceilf(X_p.y()));
	Pixel pv0, pv1, pv2, pv3;
        
	// handle edge cases
	// there are 8 cases that the a point falls out of the image boundary
	v0 = edge(v0, image);
	v1 = edge(v1, image);
	v2 = edge(v2, image);
	v3 = edge(v3, image);
	
	pv0 = Pixel(image(v0.x(), v0.y()).r(), image(v0.x(), v0.y()).g(), image(v0.x(), v0.y()).b());
	
	pv1 = Pixel(image(v1.x(), v1.y()).r(), image(v1.x(), v1.y()).g(), image(v1.x(), v1.y()).b());
	
	pv2 = Pixel(image(v2.x(), v2.y()).r(), image(v2.x(), v2.y()).g(), image(v2.x(), v2.y()).b());
	
	pv3 = Pixel(image(v3.x(), v3.y()).r(), image(v3.x(), v3.y()).g(), image(v3.x(), v3.y()).b());
		
	
	float s = X_p.x() - v0.x();
	float t = X_p.y() - v0.y(); 
	Pixel pv01 = colorInter(pv0, pv1, s);
	Pixel pv23 = colorInter(pv2, pv3, s);
	Pixel p = colorInter(pv01, pv23, t);	
	
	return p;
}


////////////////////////////////////////////////////////////////////////////////
// Code for blending two images
////////////////////////////////////////////////////////////////////////////////

void blend(Image& blendImg, const Image& first, const Image& second, float t)
{
	int minWidth = std::min(first.getWidth(), second.getWidth());
	int minHeight = std::min(first.getHeight(), second.getHeight());
	blendImg = Image(minWidth, minHeight, Pixel(0,0,0));
	for(int i = 0; i < minWidth; ++i) {
		for(int j = 0; j < minHeight; ++j) {
			// color interpolate
			Pixel p1 = first(i,j);
			Pixel p2 = second(i,j);
			Pixel result = colorInter(p1, p2, t);
			blendImg(i,j) = result;
		}
	}
	
}



// compute a warped image from a source image with two corresponding feature line segments 
// can be used to warp in both directions
// a,b and p are parameters for calculating the weight of each line segment in the feature set
// the output is a warped image of type Image
bool warpImage(Image& warpedImage, const Image& sourceImage, const FeatureSet& sourceFeatureSet, const FeatureSet& targetFeatureSet, float a, float b, float p, float t)
{
	warpedImage = Image(sourceImage.getWidth(), sourceImage.getHeight(), Pixel(0,0,0));
	for (int w = 0; w < sourceImage.getWidth(); ++w) {
		for (int h = 0; h < sourceImage.getHeight(); ++h) {
			Point X(w,h);
			Vector dSum(0,0);
			float weightSum = 0;
			for(int i = 0; i < targetFeatureSet.size(); ++i) {

				float Pi_x, Pi_y, Qi_x, Qi_y;
			
				Pi_x = interpolate(sourceFeatureSet[i].source().x(), targetFeatureSet[i].source().x(), t);
				Pi_y = interpolate(sourceFeatureSet[i].source().y(), targetFeatureSet[i].source().y(), t);
				Qi_x = interpolate(sourceFeatureSet[i].target().x(), targetFeatureSet[i].target().x(), t);
				Qi_y = interpolate(sourceFeatureSet[i].target().y(), targetFeatureSet[i].target().y(), t);
			
				Point Pi(Pi_x, Pi_y);
				Point Qi(Qi_x, Qi_y);
				Vector PiQi(Qi - Pi);
				Vector PiX(X - Pi);
				Vector perpendPiQi(-PiQi.y(), PiQi.x());
				float u = (PiX*PiQi)/(PiQi.squared_length());
				float v = (PiX*perpendPiQi)/(sqrt(PiQi.squared_length()));

				Point Pi_prime = sourceFeatureSet[i].source();
				Point Qi_prime = sourceFeatureSet[i].target();
				Vector PiQi_prime(Qi_prime - Pi_prime);
				Vector perpendPiQi_prime(-PiQi_prime.y(), PiQi_prime.x());
				Point Xi_prime(Pi_prime + (u * PiQi_prime) + (v * perpendPiQi_prime)/sqrt(PiQi_prime.squared_length()));

				Vector Di(Xi_prime - X);
				float dist;
				if (u < 0) {
					Vector p(Pi - X);
					dist = sqrt(p.squared_length()); 
				}
				else if (u > 1) {
					Vector q(Qi - X);
					dist = sqrt(q.squared_length());
				}
				else dist = abs(v);

				float weight = powf((powf(sqrt(PiQi.squared_length()), p) / (a + dist) ), b);
				dSum = dSum + Di * weight; 
				weightSum += weight; 
			}

			// the coordinate of X_prime in source image
			Point X_prime = X + dSum / weightSum; 

			// if the result pixel is out of range 
			/*if(X_prime.x() < 0 || X_prime.x() >= sourceImage.getWidth() || X_prime.y() < 0 || X_prime.y() >= sourceImage.getHeight()) {
				// four cases
				if(X_prime.x() < 0 && X_prime.y() < 0) {
					X_prime = Point(0, 0);
				}
	
				if(X_prime.x() < 0 && X_prime.y() > 0 && X_prime.y() < sourceImage.getHeight()) {
					X_prime = Point(0, X_prime.y());

				}

				if(X_prime.y() < 0 && X_prime.x() > 0 && X_prime.x() < sourceImage.getWidth()) {
					X_prime = Point(X_prime.x(), 0);

				}

				if(X_prime.x() >= sourceImage.getWidth() && X_prime.y() >= sourceImage.getHeight()) {
						
					X_prime = Point(sourceImage.getWidth()-1, sourceImage.getHeight()-1);
				}

				if(X_prime.x() >= sourceImage.getWidth() && X_prime.y() < sourceImage.getHeight()) {
					X_prime = Point(sourceImage.getWidth()-1, X_prime.y());
	
				}
				if(X_prime.x() < sourceImage.getWidth() && X_prime.y() >= sourceImage.getHeight()) {
					X_prime = Point(X_prime.x(), sourceImage.getHeight()-1);
				}

				warpedImage(w, h) = Pixel(sourceImage(X_prime.x(), X_prime.y()).r(), 
				sourceImage(X_prime.x(), X_prime.y()).g(), sourceImage(X_prime.x(), X_prime.y()).b());
				std::cout <<"w1= "<<w<<" "<<"h1= "<<h<<"\n";
			}*/

			//else {
				// assign RGB values to pixel X
                		warpedImage(w, h) = lerp(X_prime, sourceImage);
				//std::cout <<"w= "<<w<<" "<<"h= "<<h<<"\n";	
			//}
				
		}
		
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Code for morph two images
////////////////////////////////////////////////////////////////////////////////
/* compute a morphed image from two images by warping each towards the intermediate frame and then blending them
// d is the paramenter for cross-dissolving proceess
// the output is a morphed image
*/

void morphImage(Image& resultImg, const Image& sourceImage, const FeatureSet& sourceFeatureSet, const Image& targetImage, const FeatureSet& targetFeatureSet, float a, float b, float p, float t)
{
	Image warpSourceImg;
	Image warpTargetImg;
	Image morphImage;
        warpImage(warpSourceImg, sourceImage, sourceFeatureSet, targetFeatureSet, a, b, p, t);//std::cout<<"first warped\n";
        warpImage(warpTargetImg, targetImage, targetFeatureSet, sourceFeatureSet, a, b, p, 1-t);//std::cout<<"second warped\n";
	blend(resultImg, warpSourceImg, warpTargetImg, t);
      
}

///////////////////////////////////////////////////////////////////////////////
// Code for generating frames.
////////////////////////////////////////////////////////////////////////////////

// compute the digits of a number
int countDigits(int number) {
    if (number < 10) {
        return 1;
    }
    int count = 0;
    while (number > 0) {
        number /= 10;
        count++;
    }
    return count;
}

// generate file name with a given basename
bool makeFileName(std::string& fileName, std::string& basename, int index, int digits)
{
	std::ostringstream oss;
	oss << basename << std::setw(digits) << std::setfill('0') << index << ".ppm";
	fileName = oss.str();  
   
	return true;
}


