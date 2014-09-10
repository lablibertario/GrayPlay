#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxKinect.h"
#include "ofxSecondWindow.h"
#include "ofxKinectProjectorToolkit.h"
#include "ofxUI.h"
#include "ofURLFileLoader.h"

// lets do some fun stuff
#include "ofxBox2d.h"

// this must match the display resolution of your projector
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
	void draw_ctrl();
	void draw_ctrl_composite();
	void draw_debug();
	void draw_proj();
	void draw_proj_sandbox();
	void exit();
	void keyPressed(int key);
	void interactShape();

	ofxSecondWindow             secondWindow;
    
    ofxUICanvas                 *gui;
	void guiEvent(ofxUIEventArgs &e);

    ofColor backgroundColor; 
    
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
	float						ics;
	float						igrec;

	bool						isProductive;
	bool						isSandbox;
	bool						isCtrlKinect; //chose the control panel view (kinect depth/rgb | opencv / projector)
	bool						isCaptured;   //capturing kinect shape into the 2d world
	bool						isBroken;	// break triangles?
	bool						isRaining; // trhow boxes from the sky

	float						preset;
	float						contoursOnscreen;

	void urlResponse(ofHttpResponse & response);
	
	ofImage img;
	bool loading;

	ofxBox2d                            box2d;			  //	the box2d world
	
	ofPolyline							shape;			// for capture and explode
	ofPolyline							movingShape;   //  for moving blobs

	vector	  <ofPtr<ofxBox2dPolygon> >	polyShapes;		  //    pieces of shape maybe eh?

	vector    <ofPtr<ofxBox2dCircle> >	circles;		  //	default box2d circles
	vector	  <ofPtr<ofxBox2dRect> >	boxes;			  //	defalut box2d rects
	
};
