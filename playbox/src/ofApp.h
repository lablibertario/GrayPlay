#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxSecondWindow.h"
#include "ofxKinectProjectorToolkit.h"
#include "ofxUI.h"
#include "ofxColorGradient.h"
#include "ofxBox2d.h"
#include "TextureShape.h"

// this must match the display resolution of your projector (however, I did not _test_ another resolution)
#define PROJECTOR_RESOLUTION_X 1360
#define PROJECTOR_RESOLUTION_Y 768

// this is the control panel screen
#define SCREEN_RESOLUTION_X 1366
#define SCREEN_RESOLUTION_Y 768

using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void update_rain();
	void draw();
	void drawCtrl();
	void drawContoursDebug();
	void drawSelectedShape();
	void drawDebug();
	void drawProj();
	void drawGrid();
	void drawFboContours();
	
	void exit();
	void keyPressed(int key);
	void explodeShape();
	ofPoint projectedPointConvertor(int x, int y); //wtf am I doing.
	

	ofxSecondWindow             secondWindow;

	ofxUICanvas                 *gui;
	void guiEvent(ofxUIEventArgs &e);

	ofxCv::ContourFinder        contourFinder;
	ofxKinectProjectorToolkit   kpt;
	ofxKinect                   kinect;

	ofxCvGrayscaleImage         bgImage;
	ofxCvGrayscaleImage         grayImage;
	ofxCvGrayscaleImage         grayThreshNear;
	ofxCvGrayscaleImage         grayThreshFar;

	float                       nearThreshold;
	float                       farThreshold;
	float                       minArea;
	float                       maxArea;
	float                       threshold;
	float                       persistence;
	float                       maxDistance;
	int							smoothFactor,spacingFactor;

	bool						isProductive;
	bool						isGrid;
	bool						isCtrlComposite; //Compile composite image with synced projector in the control panel
	bool						isBroken;	// break triangles?
	bool						isRaining; // trhow boxes from the sky
	bool						isInteractive; // interacting with the elements
	bool						isExplosion; //used for DURING the explosion
	bool						isExplosionStart; 
	bool						isExplosionEnd; 
	bool						isGround; //box2d bottom
	bool						isGl; //opengl tests
	int							fboTrial; //which way to display fbo (ugly sandbox mode)
	bool						isFboClear; //fbo clears
	bool						isFboTrail; //fbi trails
	
	float						value; //app wide value betwwen -1 and 1 at interval of 1s
	float						piMultiplier; //gui amplifier for value
	float						preset; //preset gui var
	int							contourSelected; //the selected contour by means of keys left right :)
	int							contourSelectedPoints; //the number of points in the selected contour (why global variable, duuno because hmmm.
	float						contoursOnscreen; //total contours (at the end of update)

	// colors, baby
	ofxColorGradient			gradient;  //color scheme.
	int							colorDebug, colorContour, colorSelectedContour, colorInteractiveShape, colorBoxes, colorCircles, colorExplodingShape, colorPolishape;

	ofxBox2d                            box2d;						//	the box2d world
	ofxBox2dEdge                        movingShape;			//	the box2d edge/line shape (min 2 points)
	ofPolyline							explodingShapeLine;			// for capture and explode
	ofPolyline							movingShapeLine;			// for moving
	//vector	 <ofPtr<ofxBox2dEdge> >		movingShapes;		//to be used when multiple contours are interactive
	vector	  <ofPtr<ofxBox2dPolygon> >	polyShapes;		  //    pieces of shape maybe eh?
	vector    <ofPtr<ofxBox2dCircle> >	circles;		  //	default box2d circles
	vector	  <ofPtr<ofxBox2dRect> >	boxes;			  //	defalut box2d rects
	vector <ofImage> textures; // a vector of all the texture images
	vector <ofPtr<TextureShape> > shapes; // a vector of all the texture shapes

	//here goes gl
	
	//8 bits red, 8 bits green, 8 bits blue, from 0 to 255 in 256 steps
	ofFbo rgbaFbo; // with alpha
	//32 bits red, 32 bits green, 32 bits blue, from 0 to 1 in 'infinite' steps	
	ofFbo rgbaFboFloat; // with alpha

	int fadeAmnt;	//the vbo alpha fade thingy

	 //for the spiral (this must be somehow ina class)
	double a, b; //Angle and its increment
	ofPoint pos, lastPos; //Current and last drawing position
	ofColor color; //Drawing color
	int colorStep; //Counter for color changing

	//masking contour
	ofImage     srcImg;
    ofImage     dstImg;
    ofImage     brushImg;
    
    ofFbo       maskFbo;
    ofFbo       fbo;
    
    ofShader    shader;
    
    bool        bBrushDown;

};
