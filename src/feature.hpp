#ifndef feature_hpp
#define feature_hpp

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

using Kernal = CGAL::Simple_cartesian<float>;
using Point = Kernal::Point_2;
using Segment = Kernal::Segment_2;
using Vector = Kernal::Vector_2;
using FeatureSet = std::vector<Segment>;

// write feature line segments to DAT file
bool saveFeatureSet(std::string& file, FeatureSet& featureSegments);

// load data from a DAT file to a vector of segments
bool loadFeatureSet(const std::string& fileName, FeatureSet& featureSet);

#endif
