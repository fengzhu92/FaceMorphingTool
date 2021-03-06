# Feng Zhu
# M.Eng Porject

include ../Makefile.config

OTHER_CXXFLAGS = -g -O3 -std=c++11
CGAL_CXXFLAGS = -frounding-math
CGAL_LINKFLAGS = -lCGAL 

SPL_LINKFLAGS = -L$(SPL_TOPDIR)/lib -lSPL -lsndfile
SPL_CXXFLAGS = -I$(SPL_TOPDIR)/include
OPENGL_LINKFLAGS = -lglut -lX11 -lGL -lGLU -lm

CXX = g++
CXXFLAGS = $(OTHER_CXXFLAGS) $(CGAL_CXXFLAGS) $(SPL_CXXFLAGS)

all: select_features morph_images

image.o: image.cpp image.hpp
	$(CXX) $(CXXFLAGS) $(CGAL_CXXFLAGS) $(SPL_CXXFLAGS) -c image.cpp  

feature.o: feature.cpp feature.hpp
	$(CXX) $(CXXFLAGS) $(CGAL_CXXFLAGS) $(SPL_CXXFLAGS) -c feature.cpp  

morph.o: morph.cpp morph.hpp 
	$(CXX) $(CXXFLAGS) $(CGAL_CXXFLAGS) $(SPL_CXXFLAGS) -c morph.cpp 

select_features.o: select_features.cpp image.hpp feature.hpp
	$(CXX) $(CXXFLAGS) $(CGAL_CXXFLAGS) $(SPL_CXXFLAGS) -c select_features.cpp

select_features: select_features.o image.o feature.o
	$(CXX) $(CXXFLAGS) $(CGAL_CXXFLAGS) $(SPL_CXXFLAGS) -o $@ $^ $(CGAL_LINKFLAGS) $(OPENGL_LINKFLAGS) $(SPL_LINKFLAGS)

morph_images.o: morph_images.cpp image.hpp morph.hpp feature.hpp
	$(CXX) $(CXXFLAGS) $(CGAL_CXXFLAGS) $(SPL_CXXFLAGS) -c morph_images.cpp

morph_images: morph_images.o image.o feature.o morph.o
	$(CXX) $(CXXFLAGS) $(CGAL_CXXFLAGS) $(SPL_CXXFLAGS) -o $@ $^ $(CGAL_LINKFLAGS) $(OPENGL_LINKFLAGS) $(SPL_LINKFLAGS)

clean:
	rm -f select_features morph_images *.o
