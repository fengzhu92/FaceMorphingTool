// Feng Zhu
// Unverisity of Victoria
// Department of Electrical and Computer Engineering
// M.Eng Project


/*
   Graphical User Interface will be designed here 
   with one main window and two subwindows.

   This is a GUI drawing program written in OpenGL and GLUT.
   Input images will be displayed in two subwindows first.
   The user can draw lines in the drawing area by clicking and dragging (using the left mouse button). 
   The user can select a menu item by clicking it (with any mouse button). 
   The menu options that are implemented are Clear, Undo and Save.
   The user can exit the program by pressing 'Esc' without saving. 'Enter' for quit and save.
*/


#include <SPL/Array2.hpp>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Point_2.h>
#include <CGAL/Segment_2.h>
#include <CGAL/Vector_2.h>
#include <SPL/cgalUtil.hpp>
#include <cmath>
#include <GL/glut.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cctype>
#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include <array>
#include <iterator>
#include <fstream>
#include <algorithm>
#include "image.hpp"
#include "feature.hpp"

// declare the main window and its two subwindows
GLint window, win1, win2;
void diaplay_main();
void display_sub();

// declare some magic numbers
const float BUTTON_H = 26;
const float BUTTON_W = 40;
const float STRING_SIZE = 17;
const float STRING_FACTOR = 153;
const float STRING_Y = 7;
////////////////////////////////////////////////////////////////////////////////
// Global data type.
////////////////////////////////////////////////////////////////////////////////
struct Global
{
	Image currentImg;                      // the current image being processed
	Image drawImg;                         // the current image to be drawn 
	Segment currentSeg;			// the current line segment being drawn
	Segment leftSeg;			// the current segment on left window
	Segment rightSeg;			// the current segment on the right window
	FeatureSet currentFeature;             // the current feature set being load
	FeatureSet Feature;                    // current feature set being processed
	FeatureSet Feature1;                   // feature segments to be drawn (with window resizing ratio)
	FeatureSet Feature2;		       // feature segments to be drawn 
	std::vector<Image> images;             // images read are stored as a vector of images
	std::vector<std::string> fileNames;    // image file names to be loaded
	std::vector<std::string> givenNames;    // names to be given to output data files if provided
	std::vector<std::string> featureNames; // feaure data file names to be loaded if provided
	std::vector<FeatureSet> featureSets;   // feature sets stored as a vector of feature sets
	std::vector<int> fileFlags;            // flags indicating turning on/off operations
	int num;             // the number of input images
	int featureFlag1 = 0;// specify whether a feature set file is provided or not
	int featureFlag2 = 0;// specify whether a feature set file is provided or not
	int frameNum = 30;   // the default number of frames to be generated
	int width = 0;       // the width of input image
	int height = 0;      // the height of input image
        int windowX = 0;     // Current width of window; value is maintained by reshape().
        int windowY = 0;     // Current height of window; value is maintained by reshape().
	int lineNumber1 = 0; // Record the current number of line segments in source 
	int lineNumber2 = 0; // Record the current number of line segments in target
	int framePeriod = 11;// the frame period used when displaying with OpenGL 
        float ratioX = 1.0f;  // the X axis ratio for window reshaping
        float ratioY = 1.0f; // the Y axix ratio for window reshaping
	float ratio = 1.0f;     // for adjustment from image coordinates to entire window coordinates
	float t = 1;        //the parameter used for blending two image

	// Set to true while the user is drawing.
	bool dragging = false; 
 
	// While the user is drawing, this holds the
	// state of the modifier keys at the START
	// the drawing operation. 
	// See the documentation for glutGetModifiers().
	int dragModifiers; 
	
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
	  << "gui [-i sourceImage] [-f sourceFeatureFile] [-i tagetImage] [-f targetFeatureFile] \n"
	  << "[-i soruceImage]: the source image (PPM) to be warped\n"
	  << "[-f sourceFeatureFile]: the feature set data file for source image\n"
	  << "[-i targetImage]: the target image (PPM) to be morphed to\n"
	  << "[-f targetFeatureFile]: the feature set data file for target image\n"
	  ;
	exit(1);
}

//////////////////////////////////////////////////////////////////////////////////////
//  callback functions for source image (subWindow1)
//////////////////////////////////////////////////////////////////////////////////////


// the function is called when the user starts to draw.
void handleStartDraw(double x, double y, int modifiers) 
{
	
	// Set dragging to true to indicate drawing in progress.
	global.dragging = true;   
	global.dragModifiers = modifiers;

	x/= global.ratioX;
	y = (y - BUTTON_H)/global.ratioY + BUTTON_H;

	if (y > global.height + BUTTON_H ) {
		y = global.height + BUTTON_H;
	}
	if (y < BUTTON_H) {
		y = BUTTON_H;
	}
	if (x > global.width) {
		x = global.width;
	}
	if (x < 0) {
		x = 0;
	}

	// Set current mouse position to the starting point.
	global.currentSeg = Segment(Point(x, y - BUTTON_H),Point(x, y - BUTTON_H));
}

// This function is called when the user presses the mouse button.
void handleContinueDraw(double x, double y) 
{

	// Called by the motion() function when the user drags the mouse.
	if (!global.dragging)
		return;

	x/= global.ratioX;
	y = (y - BUTTON_H)/global.ratioY + BUTTON_H;

	if (y > global.height + BUTTON_H ) {
		y = global.height + BUTTON_H;
	}
	if (y < BUTTON_H) {
		y = BUTTON_H;
	}
	if (x > global.width) {
		x = global.width;
	}
	if (x < 0) {
		x = 0;
	}

	// This is the position of line segment
	// in the feature set that is being drawn.  
	Point p1 = global.currentSeg.source();
	global.currentSeg = Segment(p1, Point(x, y - BUTTON_H));
	
	// re-draw
	glutPostRedisplay(); 
}

// This function is called when the user releases the mouse button.
void handleFinishDraw(double x, double y) 
{
	if (!global.dragging) {
		return;
	}
	// End the draw operation.
	global.dragging = false;

	// one segments selection finished, push back to vector
	global.Feature.push_back(global.currentSeg);

	// Redraw, just in case.
	glutPostRedisplay();
 
}

// This method is called when the user clicks the menu. 
void handleMenuMouse(int x, int y, int modifiers) 
{

	if (x < BUTTON_W) {
		// Clear command; set itemCt to 0 and redraw.
		global.Feature.clear();
		global.currentSeg = Segment(Point(0,0), Point(0,0));
		// re-draw
		glutPostRedisplay();
	}
	else if (x > BUTTON_W && x < 2*BUTTON_W)  {
		// Undo command.  Decrement itemCt, is there is at least one item.
		if (global.Feature.size() > 0) {
			global.Feature.pop_back(); 
			global.currentSeg = Segment(Point(0,0), Point(0,0));
			// re-draw
			glutPostRedisplay();
		}
	}
        else if (x > 2*BUTTON_W) {
		// if no feature file is read from input, create our own
		if(global.featureNames.size() == 0) {
			std::cerr<<"data file open failed!\n";
		}  
                // if feature data file name already exists
		else {
			// save feature data to file with a given name
			saveFeatureSet(global.featureNames[glutGetWindow() - 2], global.Feature);
		}
	}
}

// Register mouse callback function 
void mouse(int button, int state, int x, int y) 
{
	// if it is the left subwindow
	// if it is the right subwindow
	if (glutGetWindow() == 2) {
		// operate on Feature1
		global.Feature = global.Feature1;
		global.currentSeg = global.leftSeg;
	}
	
	// operate on Feature2
	else if (glutGetWindow() == 3) {
		global.Feature = global.Feature2;
		global.currentSeg = global.rightSeg;
	}

        // If mouse is in the menu area
	if (x < 3*BUTTON_W && y > global.windowY && state == GLUT_DOWN) {  
		handleMenuMouse(x, global.windowY + BUTTON_H - y,glutGetModifiers());
   	}
	else if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN)
			handleStartDraw(x, global.windowY + BUTTON_H - y,glutGetModifiers());
		else
			handleFinishDraw(x,global.windowY + BUTTON_H - y);
	}
	// Update the feature set of left or right image
	// if it is the left subwindow
	// operate on Feature1
	if (glutGetWindow() == 2) {
		global.Feature1 = global.Feature;
		global.leftSeg = global.currentSeg;
	}
	// if it is the right subwindow
	// operate on Feature
	else if (glutGetWindow() == 3) {
		global.Feature2 = global.Feature;
		global.rightSeg = global.currentSeg;
	}
}

// Register motion callback function
void motion(int x, int y) 
{
	// if it is the left subwindow
	// operate on Feature1
	if (glutGetWindow() == win1) {
		global.Feature = global.Feature1;
		global.currentSeg = global.leftSeg;
	}
	// if it is the right subwindow
	// operate on Feature2
	else if (glutGetWindow() == win2) {
		global.Feature = global.Feature2;
		global.currentSeg = global.rightSeg;
	}
	if (global.dragging) {
		handleContinueDraw(x,global.windowY + BUTTON_H - y);      
	}

	// Update the feature set of left or right image
	// if it is the left subwindow
	// operate on Feature1
	if (glutGetWindow() == 2) {
		global.Feature1 = global.Feature;
		global.leftSeg = global.currentSeg;
	}
	// if it is the right subwindow
	// operate on Feature
	else if (glutGetWindow() == 3) {
		global.Feature2 = global.Feature;
		global.rightSeg = global.currentSeg;
	}
}

// function to draw line arrows on certain area
void drawItems() 
{	

	// draw the current line segment
	// the difference of coordinates
	if(global.dragging) {
	
		float deltX = global.currentSeg.target().x() - global.currentSeg.source().x();
		float deltY = global.currentSeg.target().y() - global.currentSeg.source().y();

		// X and Y axis sign
		float signX = 1.0;
		float signY = 1.0;

		// compute the angle of rotation
		float cosAlph = deltX/sqrt(pow(deltX, 2) + pow(deltY, 2));
		float sinAlph = deltY/sqrt(pow(deltX, 2) + pow(deltY, 2));
	
		// compute the coordinates of two side lines of the arrow 
		float x = cosAlph*(-8*signX);
		float y = sinAlph*(-8*signY);

		// X and Y axis sign
		if(deltX>=0) signX = -1.0;
		if(deltY>=0) signY = -1.0;

		// set the width of line 
		glLineWidth(2);
		// Draw the lines in red by default
		glColor3f(1,0,0);  
		glBegin(GL_LINES);
			glVertex2f(global.currentSeg.source().x(), global.currentSeg.source().y() + BUTTON_H);
			glVertex2f(global.currentSeg.target().x(), global.currentSeg.target().y() + BUTTON_H);
		glEnd();

		// draw the head of the arrow
		glBegin(GL_LINE_STRIP);
			glVertex2f(global.currentSeg.target().x() + (x*cos(-0.5)- y*sin(-0.5)), global.currentSeg.
			target().y() + BUTTON_H + (x*sin(-0.5)+y*cos(-0.5)));
			glVertex2f(global.currentSeg.target().x(), global.currentSeg.target().y() + BUTTON_H);
			glVertex2f(global.currentSeg.target().x() + (x*cos(0.5)- y*sin(0.5)), global.currentSeg.
			target().y() + BUTTON_H + (x*sin(0.5)+y*cos(0.5)));
		glEnd();
		
	}

	// get the number of segments in the feature set 
	global.lineNumber1 = global.Feature1.size();
	global.lineNumber2 = global.Feature2.size();

	// define an index of yellow arrow
	int yellow = -1;

	// check if the numbers of feature line segments are not equal
	if(global.lineNumber1 != global.lineNumber2) {
		if (global.Feature.size() > global.lineNumber1) {
			yellow = global.lineNumber1;
		}
		else if (global.Feature.size() > global.lineNumber2) {
			yellow = global.lineNumber2;
		}
	}

	// start drawing other lines to image
	for (int i = 0; i < global.Feature.size(); i++) {	

		// the difference of coordinates
		float deltX = global.Feature[i].target().x() - global.Feature[i].source().x();
		float deltY = global.Feature[i].target().y() - global.Feature[i].source().y();

		// X and Y axis sign
		float signX = 1.0;
		float signY = 1.0;

		// compute the angle of rotation
		float cosAlph = deltX/sqrt(pow(deltX, 2) + pow(deltY, 2));
		float sinAlph = deltY/sqrt(pow(deltX, 2) + pow(deltY, 2));
	
		// compute the coordinates of two side lines of the arrow 
		float x = cosAlph*(-8*signX);
		float y = sinAlph*(-8*signY);

		// X and Y axis sign
		if(deltX>=0) signX = -1.0;
		if(deltY>=0) signY = -1.0;

		// set the width of line 
		glLineWidth(2);
		// Draw the lines in red by default
		glColor3f(1,0,0);  
		// decide the color of the line
		// if a yellow line should be drawn
		// which means the current Feature has larger number of line segments
		if(yellow != -1) {
			if (i == yellow) {
				glColor3f(1,1,0);
			}
		}
		glBegin(GL_LINES);
			glVertex2f(global.Feature[i].source().x(), global.Feature[i].source().y() + BUTTON_H);
			glVertex2f(global.Feature[i].target().x(), global.Feature[i].target().y() + BUTTON_H);
		glEnd();

		// draw the head of the arrow
		glBegin(GL_LINE_STRIP);
			glVertex2f(global.Feature[i].target().x() + (x*cos(-0.5)- y*sin(-0.5)), global.Feature[i].
			target().y() + BUTTON_H + (x*sin(-0.5)+y*cos(-0.5)));
			glVertex2f(global.Feature[i].target().x(), global.Feature[i].target().y() + BUTTON_H);
			glVertex2f(global.Feature[i].target().x() + (x*cos(0.5)- y*sin(0.5)), global.Feature[i].
			target().y() + BUTTON_H + (x*sin(0.5)+y*cos(0.5)));
		glEnd();
	}

	
	glutPostRedisplay();
}


// This function is called to draw the string characters
void drawString(const char *str, double x=0, double y=0, double size=1.0) 
{
	glPushMatrix();
	glTranslatef(x,y,0);
	glScalef(size/STRING_FACTOR,size/STRING_FACTOR,1.0);
	int itCt = 0;
	int len = strlen(str);
	for (int i = 0; i < len; i++) {
		if (str[i] == '\n') {
			itCt++;
			glPopMatrix();
			glPushMatrix();
			glScalef(size/STRING_FACTOR,size/STRING_FACTOR,1.0);
	}
	else {
		glutStrokeCharacter(GLUT_STROKE_ROMAN,str[i]);
	}
	}
	glPopMatrix();
}


void drawMenu() 
{
	// Fill the menu area with light blue.
	glColor3f(0,0.75,1);     
	glRectf(0, 0, 3*BUTTON_W, BUTTON_H);

	// Draw lines of width 2 around and across the menu.
	glColor3f(0,0,0);    
	glLineWidth(2);
	glBegin(GL_LINES);

	// Line at the left.
	glVertex2f(1, 0);        
	glVertex2f(1, BUTTON_H + 1);
	// Line at the right.
	glVertex2f(2*BUTTON_W, 0);      
	glVertex2f(2*BUTTON_W, BUTTON_H + 1);
	// Line at the top.
	glVertex2f(0,BUTTON_H - 1);     
	glVertex2f(2*BUTTON_W,BUTTON_H - 1);
        // Line in the middle.
	glVertex2f(BUTTON_W, 0);   
	glVertex2f(BUTTON_W, BUTTON_H + 1);
        // Line at the bottom.
	glVertex2f(0, 1);     
	glVertex2f(2*BUTTON_W, 1);
	// Line at right top
        glVertex2f(2*BUTTON_W, BUTTON_H - 1);
        glVertex2f(3*BUTTON_W, BUTTON_H - 1);
	// Line at right
	glVertex2f(3*BUTTON_W, 0);
        glVertex2f(3*BUTTON_W, BUTTON_H + 1);
        // Line at bottom
        glVertex2f(2*BUTTON_W, 1);
        glVertex2f(3*BUTTON_W, 1);

	glEnd();
	// Restore line width to 1.5
	glLineWidth(1.5);  
	// Draw the command names.
	drawString("Clear", 2, STRING_Y, STRING_SIZE);  
	drawString("Undo", BUTTON_W + 2, STRING_Y, STRING_SIZE);
        drawString("Save", 2*BUTTON_W + 2, STRING_Y, STRING_SIZE);
}

// Fill the invalid area of the window
void drawInvalidArea()
{
	// Fill the menu area with light gray
	glColor3f(0.57, 0.57, 0.57);      
	glRectf(3*BUTTON_W + 1, 0, global.windowX/2,  BUTTON_H);

	// Restore line width to 2
	glLineWidth(2); 
        // Set the color to be black 
	glColor3f(0,0,0);
	// Draw the command names.
	drawString("Invalid Area", 3*BUTTON_W + 3, STRING_Y, STRING_SIZE);  
}

void drawBorder() 
{
	glColor3f(0.75, 0.75, 0.75);
	glLineWidth(4);
	glBegin(GL_LINES);

	// Line at left.
	glVertex2f(1, BUTTON_H + 1);
	glVertex2f(1, global.windowY + BUTTON_H - 1);

	// Line at right.
	glVertex2f(global.windowX/2 - 1, BUTTON_H + 1);
	glVertex2f(global.windowX/2 - 1, global.windowY + BUTTON_H - 1);

	// Line at top
	glVertex2f(1, global.windowY + BUTTON_H - 1);
	glVertex2f(global.windowX/2 - 1, global.windowY + BUTTON_H - 1);

	// Line at bottom
	glVertex2f(1, BUTTON_H + 1);
	glVertex2f(global.windowX/2 - 1, BUTTON_H + 1);

	glEnd();
}


// Sets up a coordinate system on the viewport with x ranging from
// x1 to x2 and y ranging from y1 to y2.
void initTransformation(double x1, double x2, double y1, double y2) 
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// set the coordinates
	gluOrtho2D(x1,x2,y1,y2);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// keyboard call back functions
void keyboard(unsigned char key, int x, int y) {

	// This is the escape key.
	if (key == 27)  { 
		exit(0);
	}
	// This is the enter key
	// save and quit the program
	if (key == 13) {
		saveFeatureSet(global.featureNames[0], global.Feature1);
		saveFeatureSet(global.featureNames[1], global.Feature2);
		exit(0);
	}
}

// check if file exist
bool fexists(const std::string& filename) {
	std::ifstream ifile(filename.c_str());
	return ifile;
}


void changeSize(int newX, int newY)
{
	// ratio for image
        global.ratioX = 1.0f * newX/global.width;	
	global.ratioY = 1.0f * (newY - BUTTON_H)/global.height;
	// ratio for the window
	global.ratio  = 1.0f * newY/(global.height + BUTTON_H);
       
        glutPostRedisplay();
}


// Reshape function for main window
void reshape(int newX, int newY) {
	
	// Prevent a divide by zero, when window is too short
        // (you cant make a window of zero width).
	if(newY <  BUTTON_H) {
		newY =  BUTTON_H;
	}
	if (newX <  2) {
		newX = 2;
	}
	glViewport(0, 0, newX, newY);

        // assign new values to the window size
	global.windowX = newX;
	global.windowY = newY -  BUTTON_H;

	// set subwindow1 as the current window
	glutSetWindow(win1);   

	// resize and reposition the current window
	glutPositionWindow(0, 0); 
	glutReshapeWindow(global.windowX/2, global.windowY +  BUTTON_H);

	// set the projection matrix for the current window
	changeSize(global.windowX/2, global.windowY +  BUTTON_H);
        
	// set subwindow2 as the current window
	glutSetWindow(win2);
   
	// resize and reposition the current window
	glutPositionWindow(global.windowX/2, 0); 
	glutReshapeWindow(global.windowX/2, global.windowY +  BUTTON_H);
   
	// set the projection matrix for the current window
	changeSize(global.windowX/2, global.windowY +  BUTTON_H);

}

// display function for main window
void display_main()
{
	// Set the color to use in glClear.
	glClearColor(0.0, 0.0, 0.0, 0.0);  

    	glClear(GL_COLOR_BUFFER_BIT);

	// display the window
	glutSwapBuffers();
}

// display function for the first subwindow
void display_sub() 
{

	// get the window ID
	int id = glutGetWindow();
	// if it is the left subwindow ID
	// operate on Feature1
	if (id == 2) {
		global.Feature = global.Feature1;
		global.currentSeg = global.leftSeg;
	}
	// if it is the right subwindow
	// operate on Feature2
	else if (id == 3) {
		global.Feature = global.Feature2;
		global.currentSeg = global.rightSeg;
	}

	// Set the color to use in glClear.
	glClearColor(1.0,1.0,1.0,1.0);  

	glClear(GL_COLOR_BUFFER_BIT);
   
	// Sets the part of the window used for the drawing area.
	glViewport(0, BUTTON_H, global.windowX/2, global.windowY);  
                                      
	// Establish coordinates on drawing area.
	initTransformation(0, global.windowX/2, BUTTON_H, global.windowY + BUTTON_H);  

	glMatrixMode(GL_MODELVIEW);

        // Set the raster position
	glRasterPos2i(0, BUTTON_H);

        // Set pixel zoom values
        glPixelZoom(global.ratioX, global.ratioY);

        // Draw image using glutPixelDraw
	drawImage(global.images[id - 2], 0.7);

	glPopMatrix();
	
        // Sets the part of the window used for the drawing area.
	glViewport(0, BUTTON_H,global.windowX/2,global.windowY);  
                                 
        // Establish coordinates on drawing area.
	initTransformation(0, global.windowX/2, BUTTON_H, global.windowY+BUTTON_H);  

        // draw border to the image
	drawBorder(); 

	// Sets the part of the window used for the menu.
	glViewport(0, 0, 3*BUTTON_W+1, BUTTON_H);  

	// Establish coordinates for menu -- Same as the pixel coordinates.
	initTransformation(0, 3*BUTTON_W+1 ,0, BUTTON_H); 

	// Draw the menu.
	drawMenu();  

	// Sets the part of the window used for the area.
	glViewport(3*BUTTON_W+1, 0, global.windowX/2 - 3*BUTTON_W+1, BUTTON_H);  

	// Establish coordinates for menu -- Same as the pixel coordinates.
	initTransformation(3*BUTTON_W+1, global.windowX/2 , 0, BUTTON_H); 

	// Draw the area.
	drawInvalidArea();  

	// Sets the part of the window used for the drawing area.
        glViewport(0, BUTTON_H, global.windowX/2, global.windowY);  

	// Establish coordinates on drawing area.
	initTransformation(0, global.windowX/2/global.ratioX, BUTTON_H, global.windowY/global.ratioY+BUTTON_H); 

	// Draw all the items that the user has created.
	drawItems(); 	
	
	// Makes the drawing appear on the screen!
	glutSwapBuffers();  
}


// the main entry starts here
int main(int argc, char **argv) {

	// check the validity of command line
	if(argc != 9) {
		std::cerr << "Error! Only nine command line arguments permitted.\n";
		usage();
	}

	char *cvalue = NULL;
	int c;
	opterr = 0;
	while((c = getopt(argc, argv, "i:f:")) != -1) {
		switch (c) {
			// input image file option
			case 'i':
				// get the input option
				cvalue = optarg;

				// push back the image file names
				global.fileNames.push_back(cvalue);

				// load the image as ppm file format
				loadImage(cvalue, global.currentImg, global.width, global.height);

				// push back the images to the vector of images
				global.images.push_back(global.currentImg);

				break;

			// input feature line segments option
			case 'f':

				// get the input option
				cvalue = optarg;

				// push back the feature file names
				global.featureNames.push_back(cvalue);

				// if feature data file exists
				if(fexists(cvalue)) {
					// load the feature line segments
					loadFeatureSet(cvalue, global.currentFeature);
				
					// push back the feature line segments to the vector of features sets
					global.featureSets.push_back(global.currentFeature);

					// push back the flag of feature files
                                	global.fileFlags.push_back(1);
				}
				break;


			case '?':
				if(optopt == 'i')
					std::cerr << "Option -" << optopt << "requires an argument.\n";
				if(optopt == 'f')
					std::cerr << "Option -" << optopt <<  "requires an argument.\n";
				if(isprint (optopt))
					std::cerr << "Unknow option -"<<(char)optopt<<"\n";
				else
					std::cout << "Unknown option character \\x"<< optopt <<"\n" ;
				exit(0);
			default:
				std::cerr<<"error!"<<"\n";
				exit(1);
				
		}
	}
        
	// read the number of input images
	global.num = global.images.size();

        // output the number of images	
	std::cout<<"the number of images read: "<<global.num<<"\n";

	// print out the number of feature set files
	std::cout << "the number of feature sets read: "<<global.featureSets.size() << "\n";

        // check the invalidity of the number of input images 
	if (global.num == 0) {
                // If zero, output error.
		std::cout << "No input images! Please select input source images!\n";
		exit(0);
	}
	else if (global.num == 1) {
		// If only one, output error.
		std::cout << "Only one input image. Please input two source images!\n";
		exit(0);
	}
	else if (global.num > 2) {
                // If more than two. output error.
		std::cout << "Too many input images. Please input two images at once!\n";
		exit(0);
	}

        // check if the two images are of the same size
        // if their width and height do not match, output error message
	if (global.images[0].getWidth() != global.images[1].getWidth() || global.images[0].getHeight() != global.images[1].getHeight()) 
        {
		std::cout << "Error! The size of the input images are not the same!\n";
		exit(0);
	}

	// initialize the feature set files of source and target images
        if (global.featureSets.size() == 2) {
			global.Feature1 = global.featureSets[0];
			global.Feature2 = global.featureSets[1];
			global.featureFlag1 = global.fileFlags[0]; 
			global.featureFlag2 = global.fileFlags[1];
	}
        else if (global.featureSets.size() == 1) {
		global.Feature1 = global.featureSets[0];
		global.featureFlag1 = global.fileFlags[0];
	}
	else if (global.featureSets.size() == 0) {
	}
	else {
		std::cout << "the number of feature set files should be 0, 1 or 2\n ";
		exit(0);
	}

	// if feature sets are loaded from the input 
	if (global.featureFlag1 == 1) {
		// initialize global.Feature1
		// adjust the coordinates to the  current window size
		for (int k = 0; k<global.Feature1.size();++k) {
			global.Feature1[k] = Segment(Point(global.Feature1[k].source().x(), global.Feature1[k].source().y()),
			Point(global.Feature1[k].target().x(), global.Feature1[k].target().y()));
		}
	}
	// if the feature line data file is provided from the input
	// initialize global.Feature2
	if (global.featureFlag2 == 1) {		
		for (int k = 0; k<global.Feature2.size();++k) {
			global.Feature2[k] = Segment(Point(global.Feature2[k].source().x(), global.Feature2[k].source().y()),
			Point(global.Feature2[k].target().x(), global.Feature2[k].target().y()));
		}
	
	}

	// assign the width and height of the main window
	global.windowX = 2*global.width;
        global.windowY = global.height;

        glutInit(&argc,argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
        // create the main window here
        glutInitWindowSize(global.windowX, global.windowY+ BUTTON_H);       
        glutInitWindowPosition(500,400);   
        window = glutCreateWindow("Please Select Line Segments");   

        glutDisplayFunc(display_main);    
        glutKeyboardFunc(keyboard);
        glutReshapeFunc(reshape);

        // create the first subwindow
        win1 = glutCreateSubWindow(window, 0, 0, global.windowX/2, global.windowY+ BUTTON_H);     
        glutDisplayFunc(display_sub); 
        glutMouseFunc(mouse);      
        glutMotionFunc(motion);    
        glutKeyboardFunc(keyboard);

	// create the second subwindow
        win2 = glutCreateSubWindow(window, global.windowX/2, 0, global.windowX/2, global.windowY+ BUTTON_H);   
        glutDisplayFunc(display_sub);
        glutMouseFunc(mouse);      
        glutMotionFunc(motion);    
        glutKeyboardFunc(keyboard); 
        
        // start the glut main loop here
        // this loop never returns
        glutMainLoop();
	
        return 0;
}

