#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(SCREEN_RESOLUTION_X,SCREEN_RESOLUTION_Y,OF_WINDOW);			// <-------- setup the GL context (laptop friendly)

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}