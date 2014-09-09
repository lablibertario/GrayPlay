#include "ofApp.h"

// stuff I need to remember:
// NEEDS calibration.xml
// NEEDS Verdana.ttf

void ofApp::setup() {

	//coming from box2d
	ofSetVerticalSync(true);
	ofBackgroundHex(0xfdefc2);
	ofSetLogLevel(OF_LOG_NOTICE);

	//still box2d
	box2d.init();
	box2d.setGravity(0, 10);
	//box2d.createBounds();
	//box2d.setFPS(30.0);
	//box2d.registerGrabbing();

	ofEnableAlphaBlending();
	nearThreshold = 230;
	farThreshold = 10;
	minArea = 1000;
	maxArea = 70000;
	threshold = 15;
	persistence = 15;
	maxDistance = 32;

	ofSetVerticalSync(true);

	loading=false;
	ofRegisterURLNotification(this);
    
    // set up kinect
    kinect.setRegistration(true);
	kinect.init();
	kinect.open();
    grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
    kpt.loadCalibration("calibration.xml"); 
    
    secondWindow.setup("main", ofGetScreenWidth(), 0, PROJECTOR_RESOLUTION_X, PROJECTOR_RESOLUTION_Y, true);
    
    // setup gui
    gui = new ofxUICanvas();

	gui->setFont("Verdana.ttf");                     //This loads a new font and sets the GUI font
    gui->setFontSize(OFX_UI_FONT_LARGE, 10);          //These call are optional, but if you want to resize the LARGE, MEDIUM, and SMALL fonts, here is how to do it. 
    gui->setFontSize(OFX_UI_FONT_MEDIUM, 8);           
    gui->setFontSize(OFX_UI_FONT_SMALL, 6);       
    gui->setHeight(800);
    gui->setName("parameters");
    gui->addLabel("kinect");
    gui->addSpacer();
    gui->addSlider("nearThresh", 0, 255, &nearThreshold);
    gui->addSlider("farThresh", 0, 255, &farThreshold);
    gui->addLabel("contours");
    gui->addSpacer();
    gui->addSlider("minArea", 0, 5000, &minArea);
    gui->addSlider("maxArea", 15000, 150000, &maxArea);
    gui->addSlider("threshold", 1, 100, &threshold);
    gui->addSlider("persistence", 1, 100, &persistence);
    gui->addSlider("maxDistance", 1, 100, &maxDistance);
	gui->addSpacer();
	gui->addLabel("x,y");
	gui->addSlider("x", 0, PROJECTOR_RESOLUTION_X, &ics);
	gui->addSlider("y", 0, PROJECTOR_RESOLUTION_Y, &igrec);
    gui->addSpacer();
    gui->addLabel("color");
    gui->addSlider("red", 0, 255, backgroundColor.r);
    gui->addSlider("green", 0, 255, backgroundColor.g);
    gui->addSlider("blue", 0, 255, backgroundColor.b);

	// enable autoload
	//gui->loadSettings("gui1.xml");
	 
	// added to catch real time update of color
	ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);
}

void ofApp::update() {

	box2d.update();	

    kinect.update();    
    if(kinect.isFrameNew()) {
        // process kinect depth image
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        grayThreshNear = grayImage;
        grayThreshFar = grayImage;
        grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar.threshold(farThreshold);
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        grayImage.flagImageChanged();
		
        // set contour tracker parameters
		contourFinder.setMinArea(minArea);
        contourFinder.setMaxArea(maxArea);
        contourFinder.setThreshold(threshold);
        contourFinder.getTracker().setPersistence(persistence);
        contourFinder.getTracker().setMaximumDistance(maxDistance);
        
        // determine found contours
        contourFinder.findContours(grayImage);
		
		//framerate
		ofSetWindowTitle( ofToString( ofGetFrameRate(),1 ) );
	}
}

void ofApp::draw() {
    // GUI
	ofBackground(0); 
	ofSetColor(255);
	ofPushMatrix();  //push the motherfing matrix oh yeah!

	if (isProductive) {
		//CV meagic (abracadabric - sunt eu sub impresia ca ar consuma multe resurse chestia asta si oricum o fac pt projector mai jos, sa no mai fac every time here)
		RectTracker& tracker = contourFinder.getTracker();

		// draw kinectRGB,depth,contours,composite display		
		kinect.draw(0, 0); 
		ofTranslate(640, 0);
		grayImage.draw(0, 0); 
		ofTranslate(-640, 480);
		contourFinder.draw();
		ofTranslate(640, 0); 

		// draw composite
		for(int i = 0; i < contourFinder.size(); i++) {
			vector<cv::Point> points = contourFinder.getContour(i);
			int label = contourFinder.getLabel(i);
			ofPoint center = toOf(contourFinder.getCenter(i));
			int age = tracker.getAge(label);        
			ofDrawBitmapString(ofToString(label) + " " + ofToString(age) , center.x, center.y);

			// map contour using calibration and draw to control panel window 
			ofBeginShape();
			ofFill();
			ofSetColor(backgroundColor); // backgroundColor // blobColors[0] //label % 11
			for (int j=0; j<points.size(); j++) {
				ofVec3f wp = kinect.getWorldCoordinateAt(points[j].x, points[j].y);
				ofVec2f pp = kpt.getProjectedPoint(wp);          
				// this gentleman right here is the projection
				ofVertex(
					ofMap(pp.x, 0, 1, 0, 640),
					ofMap(pp.y, 0, 1, 0, 480)
					);
			}
			ofEndShape();  

			// map contour uncalibrated (dimmed 50%)
			ofEnableAlphaBlending();
			ofBeginShape();
			ofFill();
			ofSetColor(backgroundColor,127); // backgroundColor // blobColors[0] //label % 11 // man that's awesome this be the line that was the most awe from awesome. because colors, so fuck off.
			for (int j=0; j<points.size(); j++) {
				ofVec3f wp = kinect.getWorldCoordinateAt(points[j].x, points[j].y);
				ofVec2f pp = kpt.getProjectedPoint(wp);          
				// this gentleman right here is the real shit
				ofVertex( points[j].x, points[j].y);
			}
			ofEndShape();  
			ofDisableAlphaBlending();  
		}
	} // end if isProductive

	ofPopMatrix(); 

	draw_proj();
}

void ofApp::draw_proj() {

	//CV meagic (abracadabric)
	RectTracker& tracker = contourFinder.getTracker();
	
    //projector
    secondWindow.begin();


    ofBackground(0);    

	//Drawing background	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	if(loading)
		ofDrawBitmapString("loading...", 10, ofGetHeight()+20);
	if(img.bAllocated()){
		ofSetColor(255);
		img.draw(0,0,secondWindow.getWidth(), secondWindow.getHeight());	
	}
    
	// gui controlled position shape
	ofPushMatrix();
    ofTranslate(ics, igrec);    
    ofBeginShape();
    ofFill();
	ofSetColor(backgroundColor); 
	ofVertex(400,135);
	ofVertex(215,135);
	ofVertex(365,25);
	ofVertex(305,200);
	ofVertex(250,25);
	ofEndShape();
	ofPopMatrix(); 
	
	
    for(int i = 0; i < contourFinder.size(); i++) {
        vector<cv::Point> points = contourFinder.getContour(i);
        int label = contourFinder.getLabel(i);
        ofPoint center = toOf(contourFinder.getCenter(i));
		int age = tracker.getAge(label);


		float w = ofRandom(20, 30);
		float h = ofRandom(20, 30);
		boxes.push_back(ofPtr<ofxBox2dRect>(new ofxBox2dRect));
		boxes.back().get()->setPhysics(3.0, 0.53, 0.1);
		boxes.back().get()->setup(box2d.getWorld(), center.x, center.y, w, h);


  //      // map contour using calibration and draw to main window
  //      ofBeginShape();
  //      ofFill();
		//ofSetColor(backgroundColor); // backgroundColor // blobColors[0] //label % 11
  //      for (int j=0; j<points.size(); j++) {
  //          ofVec3f wp = kinect.getWorldCoordinateAt(points[j].x, points[j].y);
  //          ofVec2f pp = kpt.getProjectedPoint(wp);         
		//	// this gentleman right here is the projection
		//	  ofVertex(
  //                     ofMap(pp.x, 0, 1, 0, secondWindow.getWidth()),
  //                     ofMap(pp.y, 0, 1, 0, secondWindow.getHeight())
  //                    );
  //      }
  //      ofEndShape(); 


    }    
    
	
	for(int i=0; i<boxes.size(); i++) {
		ofFill();
		ofSetHexColor(0xBF2545);
		boxes[i].get()->draw();
	}

	// draw the ground
	box2d.drawGround();

	secondWindow.end();

}

void ofApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
	
    if(name == "red")
	{
		ofxUISlider *rslider = (ofxUISlider *) e.widget; 
		backgroundColor.r = rslider->getScaledValue(); 		
	}    
	else if(name == "green")
	{
		ofxUISlider *rslider = (ofxUISlider *) e.widget; 
		backgroundColor.g = rslider->getScaledValue(); 		
	}    
	else if(name == "blue")
	{
		ofxUISlider *rslider = (ofxUISlider *) e.widget; 
		backgroundColor.b = rslider->getScaledValue(); 		
	}    
   
    
}

void ofApp::keyPressed(int key){

// if I ever need to get out pressing keys (like an input field in gui)	
//    if(gui2->hasKeyboardFocus())
//    {
//       return;
//    }
	
	switch (key)
	{
		case 'p':
			isProductive=!isProductive;
			break;

		case 'f':
			ofToggleFullscreen();
			break;
        			
        case '1':
            gui->loadSettings("gui1.xml");
            break;
            
        case '2':
            gui->loadSettings("gui2.xml");
            break;
            
        case '3':
            gui->loadSettings("gui3.xml");
            break;
            
        case '4':
            gui->loadSettings("gui4.xml");
            break;
            
		case OF_KEY_F1:
            gui->saveSettings("gui1.xml");
            break;
            
        case OF_KEY_F2:
            gui->saveSettings("gui2.xml");
            break;
            
        case OF_KEY_F3:
            gui->saveSettings("gui3.xml");
            break;
            
        case OF_KEY_F4:
            gui->saveSettings("gui4.xml");
            break;

		case 'l':
			img.clear();
			ofLoadURLAsync("https://scontent-b-fra.xx.fbcdn.net/hphotos-xfp1/l/t31.0-8/10619982_281873851999198_2117754016354577419_o.jpg","ziua"); // make an array, add to settings (presets) and config file.
			loading =true;
			break;


		default:
			break;
	}
}

void ofApp::urlResponse(ofHttpResponse & response){
	if(response.status==200 && response.request.name == "ziua"){
		img.loadImage(response.data);
		loading=false;
	}else{
		cout << response.status << " " << response.error << endl;
		if(response.status!=-1) loading=false;
	}
}

void ofApp::exit()
{
    //enable autosave
	//gui->saveSettings("gui1.xml");  

	delete gui;	
}