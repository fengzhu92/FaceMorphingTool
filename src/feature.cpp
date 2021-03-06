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
#include "feature.hpp"

// write feature line segments to DAT file
bool saveFeatureSet(std::string& file, FeatureSet& featureSegments) 
{
	std::ofstream fout(file.c_str());
	if(fout.is_open()) {
		// first line records the number of features in the file
		fout << featureSegments.size() << "\n";
		// remained lines are the line segments 
		for(int it = 0; it < featureSegments.size(); ++it) {
			fout << int(featureSegments[it].source().x()) <<" "<< int(featureSegments[it].source().y()) <<" "
				<< int(featureSegments[it].target().x()) << " "<<int(featureSegments[it].target().y())<< "\n";
		}
		fout.close();
	}
	else {
		std::cerr << "File Open Failed: "<<file<<"\n";
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Read feature line segmentsgenerate filenames with  C++
////////////////////////////////////////////////////////////////////////////////

// load data from a DAT file to a vector of segments
bool loadFeatureSet(const std::string& fileName, FeatureSet& featureSet)
{
	std::string line;
	std::vector<Segment> feature;
	std::fstream file(fileName.c_str());
	if(file.is_open())
	{
		// skip the first line
		std::getline(file, line);
		// read in each line of file
		while(std::getline(file, line)) {
			// copy the read in string to a istreamstring
			std::istringstream lineStream(line);
			int p1x, p1y, p2x, p2y;

			// read in the coordinates of endpoints
			lineStream >> p1x;
			lineStream >> p1y;
			lineStream >> p2x;
			lineStream >> p2y;

			// form a line segment
			Segment lineSegment = Segment(Point(p1x, p1y), Point(p2x, p2y));
			// push back the line segment in to a vector
			feature.push_back(lineSegment);		
		}
		featureSet = feature;
	        // close the file	
		file.close();		
	}
	else {
		std::cerr << "Unable to open line segment file: " << fileName << "\n";
		return false;
	}
	return true;	
}














