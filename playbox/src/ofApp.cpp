#include "ofApp.h"

void ofApp::setup() {

	//coming from box2d
	ofDisableAntiAliasing();
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_NOTICE); //not used but who knows.
	box2d.init();
	box2d.setGravity(0, 10);
	box2d.createGround();
	box2d.setFPS(30.0);
	
	//some flags controlling states of things
	loading=false;
	isProductive=false;
	isSandbox=true;
	ofRegisterURLNotification(this);
	    
    // set up kinect
    kinect.setRegistration(true);
	kinect.init();
	kinect.open();

	// ofxCvGrayscaleImage
    grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	//kinectprojectortoolkit 
    kpt.loadCalibration("calibration.xml"); 
    
	//default hard coded parameters. why oh why
	ofEnableAlphaBlending();
	nearThreshold = 230;
	farThreshold = 10;
	minArea = 1000;
	maxArea = 70000;
	threshold = 15;
	persistence = 15;
	maxDistance = 32;
	preset=1;
	isProductive=false;
	isSandbox=true;

    // setup gui that's why (must be a way to set them up directly in the gui hmmmm)
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
	gui->addSpacer();
	gui->addLabelToggle("isProductive", &isProductive);
	gui->addLabelToggle("isSandbox", &isSandbox);
	gui->addSlider("preset", 1, 4, &preset);

	// enable autoload
	//gui->loadSettings("gui1.xml");
	 
	// added to catch real time update of color
	ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);

	//setup performance window
	secondWindow.setup("main", ofGetScreenWidth(), 0, PROJECTOR_RESOLUTION_X, PROJECTOR_RESOLUTION_Y, true);
}

void ofApp::update() {
	//show framerate
	ofSetWindowTitle( ofToString( ofGetFrameRate(),1 ) );

	// todo: should remove stuff first, before updating box2d
	box2d.update();	

	//update the damn thing
    kinect.update();    

    if(kinect.isFrameNew()) {
        // get depth image (1)
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);

		//cut the'range' of grays by making (2) images with low and upper threshold
        grayThreshNear = grayImage;
		grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar = grayImage;
        grayThreshFar.threshold(farThreshold);

		//run cv on the (3) images
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
	}
}

void ofApp::draw() {
	// using two windows is confusing, so i'm trying to break it in 3 :)
	// stuff that has to happen regardless (why is it not at the update?)

	//draw stuff in the first window only when needed (save speed)
	if (!isProductive) { draw_ctrl(); }

	//draw the projector stuff.
	draw_proj();
}

void ofApp::draw_ctrl() {
	// welcome to the first screen, the control pannel
	// one day this would be moved to a complete separate process that would address the production window by remote. one can only dream :)

	//old skool mood for ctrl.
	ofBackground(0); 
	ofSetColor(255);

	// draw kinectRGB,depth,contours,composite display		
	ofPushMatrix();  
	kinect.draw(0, 0); 
	ofTranslate(640, 0);
	grayImage.draw(0, 0); 
	ofTranslate(-640, 480);
	contourFinder.draw();
	ofTranslate(640, 0); 

	
	// draw composite
	RectTracker& tracker = contourFinder.getTracker();
	for(int i = 0; i < contourFinder.size(); i++) {
		vector<cv::Point> points = contourFinder.getContour(i);
		int label = contourFinder.getLabel(i);
		ofPoint center = toOf(contourFinder.getCenter(i));
		int age = tracker.getAge(label);        
		ofDrawBitmapString(ofToString(label) + " " + ofToString(age) , center.x, center.y);

		// map contour using calibration and draw to control panel window 
		ofBeginShape();
		ofFill();
		ofSetColor(backgroundColor); 
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
	ofPopMatrix(); 
}

void ofApp::draw_proj() {
	// welcome to the second screen, the projector/production/play
	// one day this will be the main screen after some kind of network pipeline with the ctrl

	//projector
    secondWindow.begin();

	//change of mood
	ofBackgroundHex(0xfdefc2);

	if (isSandbox) { draw_proj_sandbox(); };

	//now let's see what the hack can we do with boxes and polygons and what not.
	//CV meagic (abracadabric)
	RectTracker& tracker = contourFinder.getTracker();

    for(int i = 0; i < contourFinder.size(); i++) {
        vector<cv::Point> points = contourFinder.getContour(i);
        int label = contourFinder.getLabel(i);
        ofPoint center = toOf(contourFinder.getCenter(i));
		int age = tracker.getAge(label);


		//float w = ofRandom(20, 30);
		//float h = ofRandom(20, 30);
		//boxes.push_back(ofPtr<ofxBox2dRect>(new ofxBox2dRect));
		//boxes.back().get()->setPhysics(3.0, 0.53, 0.1);
		//boxes.back().get()->setup(box2d.getWorld(), center.x, center.y, w, h);


        // map contour using calibration and draw to main window
        ofBeginShape();
        ofFill();
		ofSetColor(backgroundColor); // backgroundColor // blobColors[0] //label % 11
        for (int j=0; j<points.size(); j++) {
            ofVec3f wp = kinect.getWorldCoordinateAt(points[j].x, points[j].y);
            ofVec2f pp = kpt.getProjectedPoint(wp);         
			// this gentleman right here is the projection
			  ofVertex(
                       ofMap(pp.x, 0, 1, 0, secondWindow.getWidth()),
                       ofMap(pp.y, 0, 1, 0, secondWindow.getHeight())
                      );
        }
        ofEndShape(); 
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

void ofApp::draw_proj_sandbox() {

	// attepmpt 1.0 - image background
	//Drawing background	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	if(loading)
		ofDrawBitmapString("loading...", 10, ofGetHeight()+20);
	if(img.bAllocated()){
		ofSetColor(255);
		img.draw(0,0,secondWindow.getWidth(), secondWindow.getHeight());	
	}
    
	// attepmpt 2.0 - some moving shape controlled in ctlr (ics, igrec)
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

		case 'a':
			isSandbox=!isSandbox;
			break;

		case 'f':
			ofToggleFullscreen();
			break;
        			
        case '1':
            gui->loadSettings("gui1.xml");
			preset=1;
            break;
            
        case '2':
            gui->loadSettings("gui2.xml");
			preset=2;
            break;
            
        case '3':
            gui->loadSettings("gui3.xml");
			preset=3;
            break;
            
        case '4':
            gui->loadSettings("gui4.xml");
			preset=4;
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