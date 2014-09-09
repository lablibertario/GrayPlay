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


	using namespace ofxCv;
	using namespace cv;


class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();
	void draw_proj();
	void exit();
	void keyPressed(int key);
//    void keyReleased(int key);

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

	void urlResponse(ofHttpResponse & response);
	
	ofImage img;
	bool loading;

	ofxBox2d                            box2d;			  //	the box2d world
	vector    <ofPtr<ofxBox2dCircle> >	circles;		  //	default box2d circles
	vector	  <ofPtr<ofxBox2dRect> >	boxes;			  //	defalut box2d rects

	
};
