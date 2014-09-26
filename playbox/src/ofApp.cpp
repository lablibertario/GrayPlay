#include "ofApp.h"

static bool shouldRemove(ofPtr<ofxBox2dBaseShape>shape) {
	return !ofRectangle(0, -200, ofGetWidth(), ofGetHeight()+200).inside(shape.get()->getPosition());
}

void ofApp::setup() {

	//coming from box2d
	ofDisableAntiAliasing();
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_NOTICE); //not used but who knows.
	box2d.init();
	box2d.setGravity(0, 10);
	box2d.createGround(); // this need to be added as a box2d shape (on/off)
	box2d.setFPS(30.0);
	
	// dynamic load ? (cleanup)
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
    
	//flags
	loading=false;
	isProductive=false;
	isProductive=false;
	isContours=true;
	isCtrlComposite=true;
	isBroken=true;
	isRaining=false;
	isInteractive=false;
	isExplosion=false;
	isGround=true;

	//default hard coded parameters. why oh why
	// ofEnableAlphaBlending(); wtf!
	nearThreshold = 230;
	farThreshold = 10;
	minArea = 1000;
	maxArea = 70000;
	threshold = 15;
	persistence = 15;
	maxDistance = 32;
	preset=1;
	contourTimer=0;
	piMultiplier=1.0f;

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
	gui->addLabel("Project");
	gui->addSlider("preset", 1, 4, &preset); //no ideea how to make a radio button
	//gui->addSlider("contour", 1, 10, &contourSelected); //no ideea how to make a radio button
	gui->addSpacer();
	gui->addLabelToggle("isProductive", &isProductive);
	gui->addLabelToggle("isCtrlComposite", &isCtrlComposite);
	gui->addSpacer();
	gui->addLabelToggle("isContours", &isContours);
	gui->addLabelToggle("isInteractive", &isInteractive);
	gui->addSpacer();
	gui->addLabelToggle("isRaining", &isRaining);
	gui->addLabelToggle("isGround", &isGround);
	gui->addLabelToggle("isBroken", &isBroken);
	gui->addLabelToggle("isExplosion", &isExplosion);
	gui->addSpacer();
	gui->addSlider("contourTimer", 0, 2000, &contourTimer); //how fast to refresh interactive shape
	gui->addSlider("piMultiplier", 0.0f, 2.0f, &piMultiplier); //how fast to loop through the colors
	;
	//gui->addLabel("x,y");
	//gui->addSlider("x", 0, PROJECTOR_RESOLUTION_X, &ics);
	//gui->addSlider("y", 0, PROJECTOR_RESOLUTION_Y, &igrec);
    //gui->addSpacer();
    //gui->addLabel("color");
    //gui->addSlider("red", 0, 255, backgroundColor.r);
    //gui->addSlider("green", 0, 255, backgroundColor.g);
    //gui->addSlider("blue", 0, 255, backgroundColor.b);
	
	// enable autoload
	//gui->loadSettings("gui1.xml");
	 
	// added to catch real time update of color
	ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);

	//colorPlay
	colorDebug = 0xFCFAE1;
	colorContour = 0xFCFAE1;
	colorSelectedContour = 0xFCFAE1;
	colorInteractiveShape = 0xFCFAE1;
	colorBoxes = 0xFCFAE1;
	colorCircles = 0xFCFAE1;
	colorExplodingShape = 0xFCFAE1;
	colorPolishape = 0xFCFAE1;

	//custom gradient design
	//gradient.addColor( ofColor::black );
	//gradient.addColor( ofColor::white );
	//gradient.addColor( ofColor::black );
	
	////theme 1
	//ofColor c;
	//c.setHex(0xE6E2AF); gradient.addColor(c);
	//c.setHex(0xA7A37E); gradient.addColor(c);
	//c.setHex(0xEFECCA); gradient.addColor(c);
	//c.setHex(0x046380); gradient.addColor(c);
	//c.setHex(0x002F2F); gradient.addColor(c);
	//c.setHex(0xE6E2AF); gradient.addColor(c); // <--back to the first one
	//gradienti=0.0f;

	//theme 2
	ofColor c;
	c.setHex(0xFF6138); gradient.addColor(c);
	c.setHex(0xFFFF9D); gradient.addColor(c);
	c.setHex(0xBEEB9F); gradient.addColor(c);
	c.setHex(0x79BD8F); gradient.addColor(c);
	c.setHex(0x00A388); gradient.addColor(c);
	c.setHex(0xFF6138); gradient.addColor(c); // <--back to the first one
	

	//setup performance window
	secondWindow.setup("main", ofGetScreenWidth(), 0, PROJECTOR_RESOLUTION_X, PROJECTOR_RESOLUTION_Y, true);

	// here goes
	fbo.allocate( PROJECTOR_RESOLUTION_X, PROJECTOR_RESOLUTION_Y );

	//fix this with the pi thingy.
	timer1 = ofGetElapsedTimeMillis();
	timer2 = ofGetElapsedTimeMillis();
}

void ofApp::update() {

	//get time to be used for sin functions lateron
	float time = ofGetElapsedTimef(); 
	//Get periodic value in [-1,1], with wavelength equal to 0.5 seconds
	value = sin( time * PI * piMultiplier);

	// map value from -1,1 to 0.0f 0.1f
	float gradienti = ofMap(value, -1, 1, 0.0f, 1.0f );

	//update the color of the contour to loop gradiently
	colorContour = gradient.getColorAtPercent(gradienti).getHex();

	//fix conturselected if it goes outa wack
	if (contourSelected>(contoursOnscreen-1)) { contourSelected = contoursOnscreen-1; }
	if (contourSelected<0) { contourSelected = 0; }

	//show framerate
	ofSetWindowTitle( ofToString( ofGetFrameRate(),1 ) );

	//cleanup
	ofRemove(circles, shouldRemove);
	ofRemove(boxes, shouldRemove);
	ofRemove(polyShapes, shouldRemove);

	//updating box2d
	box2d.update();	
	
	// rain
	if (isRaining) { update_rain();	}

	// transform ONE countour into interactive box2s shapeline ever half a second (UPDATE)
	if ((isInteractive)&&(contoursOnscreen>0)&&(ofGetElapsedTimeMillis()-timer2>contourTimer)) { 
		vector<cv::Point> points = contourFinder.getContour(contourSelected);
		movingShapeLine.clear(); 
		for (int j=0; j<points.size(); j++) {
			ofVec3f wp = kinect.getWorldCoordinateAt(points[j].x, points[j].y);
			ofVec2f pp = kpt.getProjectedPoint(wp);         
			// this gentleman right here is the projection
			movingShapeLine.addVertex(
				ofMap(pp.x, 0, 1, 0, secondWindow.getWidth()),
				ofMap(pp.y, 0, 1, 0, secondWindow.getHeight())
				);
		}
		movingShapeLine.simplify(); // (fixthis)

		//take the moving shape lines and make a movigshape with it, then create it in the getworld (will be always updated in draw? then again created here my god this is confusing.. anyways)
		movingShape.clear();
		movingShape.addVertexes(movingShapeLine);
		movingShape.create(box2d.getWorld());
		timer2 = ofGetElapsedTimeMillis();
	}
	

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

		//get a fresh variable for all those questions about this deep in this cycle
		contoursOnscreen = contourFinder.size();
	}
}

void ofApp::update_rain(){
	// add some circles and boxes every random interval from 500ms up to a 1 sec
	if(ofGetElapsedTimeMillis()-timer1>(int)ofRandom(500, 1000)){
		ofPtr<ofxBox2dCircle> circle = ofPtr<ofxBox2dCircle>(new ofxBox2dCircle);
		circle.get()->setPhysics(0.3, 0.5, 0.1);
		circle.get()->setup(box2d.getWorld(), (ofGetWidth()/2)+ofRandom(-20, 20), -20, ofRandom(10, 30));
		circles.push_back(circle);
		timer1 = ofGetElapsedTimeMillis();
	}
	if(ofGetElapsedTimeMillis()-timer1>(int)ofRandom(500, 1000)){
		ofPtr<ofxBox2dRect> box = ofPtr<ofxBox2dRect>(new ofxBox2dRect);
		box.get()->setPhysics(0.3, 0.5, 0.1);
		box.get()->setup(box2d.getWorld(), (ofGetWidth()/2)+ofRandom(-20, 20), -20, ofRandom(10, 40), ofRandom(10, 40));
		boxes.push_back(box);
		timer1 = ofGetElapsedTimeMillis();
	}
}

void ofApp::draw() {
	// using two windows is confusing, so i'm trying to break it in 3 :)
	
	
	//draw complicated stuff in the first window only when needed (save speed)
	if (!isProductive) { drawCtrl(); }
	
	// draw some simple stuff in the first window
	drawDebug();

	//draw the projector stuff.
	drawProj();
}

void ofApp::drawDebug() {
	// some debug information
	string info = "Keys:\n";
	info += "(p) "+string(isProductive?"Production":"Edit")+"\n";
	info += "(o) "+string(isCtrlComposite?"Kinect":"OpenCV")+"\n";
	info += "(i) "+string(isInteractive?"Box2d":"Shape")+"\n";
	info += "(r) "+string(isRaining?"Raining":"Clear")+"\n";
	info += "(b) to break shape\n\n";
	
	info += "Total Contours: "+ofToString(contoursOnscreen)+"\n";
	info += "Contour slected: "+ofToString(contourSelected)+"\n";
	info += "Total Bodies: "+ofToString(box2d.getBodyCount())+"\n\n";
	info += "FPS: "+ofToString(ofGetFrameRate())+"\n";
	ofSetHexColor(colorDebug);
	ofDrawBitmapString(info, 300, 550);
}

void ofApp::drawCtrl() {
	// welcome to the first screen, the control pannel
	// one day this would be moved to a complete separate process that would address the production window by remote. one can only dream :)

	//old skool mood for ctrl.
	ofBackground(0); 
	ofSetColor(255);
	
	//store transformation matrix and start drawing windows
	ofPushMatrix();  
		
	// draw kinectRGB + contours  / gray image + proj composite
	ofEnableAlphaBlending(); 
	kinect.draw(0, 0); 
	contourFinder.draw();	
	ofTranslate(640, 0);
	grayImage.draw(0, 0); 
	ofDisableAlphaBlending(); 

	// debug projector screen (could slow down)
	if (isCtrlComposite) {
		//draw contrours in a small window with debug
		drawContours(640,480,true);
	}

	// what goes up must come down
	ofPopMatrix();
}

void ofApp::drawContours(int width, int height, bool debugProjector) {
	// draw contours projected
	ofEnableAntiAliasing();
	ofEnableSmoothing();
	for(int i = 0; i < contoursOnscreen; i++) {

		vector<cv::Point> points = contourFinder.getContour(i);
		int label = contourFinder.getLabel(i);
		 cout << "nr of points " << points.size() << endl;
		// draw contours using kinectoolkit conversion
		ofBeginShape();
		ofFill();
		ofSetHexColor(colorContour); 


		for (int j=0; j<points.size(); j++) {
			ofVec3f wp = kinect.getWorldCoordinateAt(points[j].x, points[j].y);
			ofVec2f pp = kpt.getProjectedPoint(wp);         
			// this gentleman right here is the projection
			ofVertex(
				ofMap(pp.x, 0, 1, 0, width),
				ofMap(pp.y, 0, 1, 0, height)
				);
		}
		ofEndShape(); 

		if (debugProjector)
		{
			ofSetColor(yellowPrint);
			ofPolyline convexHull = toOf(contourFinder.getConvexHull(i));
			convexHull.draw();

			ofEnableAlphaBlending();
			ofBeginShape();
			ofFill();
			//mark the selected contour
			if (contourSelected==i) { ofSetHexColor(colorSelectedContour); } else { ofSetHexColor(colorContour); }
			for (int j=0; j<points.size(); j++) {
				// this gentleman right here is the real shit
				ofVertex( points[j].x, points[j].y);
			}
			ofEndShape();  
			ofDisableAlphaBlending();
		}
	}    	
	ofDisableAntiAliasing();
	ofDisableSmoothing();
}

void ofApp::drawProj() {
	// welcome to the second screen, the projector/production/play
	// one day this will be the main screen after some kind of network pipeline with the ctrl
	//projector
    secondWindow.begin();
	
	//Map value from [-1,1] to [0,255]
	// float v = ofMap( value, -1, 1, 0, 255 );
	//background loops
	//ofBackground(v,v,v);
	ofBackground(0);

	//CV meagic (abracadabric)
	RectTracker& tracker = contourFinder.getTracker();
	
	//draw contours without debug.
	if (isContours) { 
		drawContours(secondWindow.getWidth(),secondWindow.getHeight(),false); 
	}

	if (isInteractive) {
		// draw the moving shape - drawing line is exactly the same wtf (debug)
		//ofSetHexColor(0x6D130E);
		//movingShapeLine.draw();
		movingShape.updateShape();
		ofSetHexColor(colorInteractiveShape);
		movingShape.draw();
	}
	
	if ((isRaining)||(boxes.size()>0)||(circles.size()>0)) { 
		// draw boxes
		for(int i=0; i<boxes.size(); i++) {
			ofFill();
			ofSetHexColor(colorBoxes);
			boxes[i].get()->draw();
		}
		// draw cicles
		for (int i=0; i<circles.size(); i++) {
			ofFill();
			ofSetHexColor(colorCircles);
			circles[i].get()->draw();
		}
	}

	if (isExplosion) {
		//draw the exploding shape 
		ofSetHexColor(colorExplodingShape);
		ofFill();
		explodingShapeLine.draw();

		//draw the polishape
		ofSetHexColor(colorPolishape);
		ofFill();
		for (int i=0; i<polyShapes.size(); i++) {
			polyShapes[i].get()->draw();        
			//ofCircle(polyShapes[i].get()->getPosition(), 3);  //a small circle inside the triangle so cute but NO.
		}	
	}

	// draw the ground (fixthis)
	//if (isGround) { box2d.createGround(); }
	
	//close secondwindow
	secondWindow.end();
}

void ofApp::explodeShape() {

	//clear the line that will be exploded
	explodingShapeLine.clear();  

	//gets contour 1
	vector<cv::Point> points = contourFinder.getContour(contourSelected);

	//for all points of the contour
	for (int j=0; j<points.size(); j++) {
		ofVec3f wp = kinect.getWorldCoordinateAt(points[j].x, points[j].y);
		ofVec2f pp = kpt.getProjectedPoint(wp);         
		// add projected
		explodingShapeLine.addVertex( 
			ofMap(pp.x, 0, 1, 0, secondWindow.getWidth()),
			ofMap(pp.y, 0, 1, 0, secondWindow.getHeight())
			);
	}

	if (isBroken) { //explode!
		// first simplify the shape
		explodingShapeLine.simplify();

		// save the outline of the shape
		ofPolyline outline = explodingShapeLine;

		// resample shape
		ofPolyline resampled = explodingShapeLine.getResampledBySpacing(25);

		// trangleate the shape, return am array of traingles
		vector <TriangleShape> tris = triangulatePolygonWithOutline(resampled, outline);

		// add some random points inside
		addRandomPointsInside(explodingShapeLine, 255);

		// now loop through all the trainles and make a box2d triangle
		for (int i=0; i<tris.size(); i++) {
			ofPtr<ofxBox2dPolygon> triangle = ofPtr<ofxBox2dPolygon>(new ofxBox2dPolygon);
			triangle.get()->addTriangle(tris[i].a, tris[i].b, tris[i].c);
			triangle.get()->setPhysics(1.0, 0.3, 0.3);
			triangle.get()->create(box2d.getWorld());
			polyShapes.push_back(triangle);
		}
	} else { // drop a poly shape 
		
		// first simplify the shape
		explodingShapeLine.simplify();

		//explodingShapeLine = explodingShapeLine.getResampledByCount(b2_maxPolygonVertices );
		explodingShapeLine = explodingShapeLine.getResampledBySpacing(25);

		//explodingShapeLine = getConvexHull(explodingShapeLine);

		ofPtr<ofxBox2dPolygon> poly = ofPtr<ofxBox2dPolygon>(new ofxBox2dPolygon);
		poly.get()->addVertices(explodingShapeLine.getVertices());
		poly.get()->setPhysics(1.0, 0.3, 0.3);
		poly.get()->create(box2d.getWorld());
		polyShapes.push_back(poly);
	}
	// done with shape clear it now
	explodingShapeLine.clear();

}

void ofApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 
	
 //   if(name == "red")
	//{
	//	ofxUISlider *rslider = (ofxUISlider *) e.widget; 
	//	backgroundColor.r = rslider->getScaledValue(); 		
	//}    
	//else if(name == "green")
	//{
	//	ofxUISlider *rslider = (ofxUISlider *) e.widget; 
	//	backgroundColor.g = rslider->getScaledValue(); 		
	//}    
	//else if(name == "blue")
	//{
	//	ofxUISlider *rslider = (ofxUISlider *) e.widget; 
	//	backgroundColor.b = rslider->getScaledValue(); 		
	//}    
   
    
}

void ofApp::keyPressed(int key){

	// if I ever need to get out pressing keys (like an input field in gui)	
	//    if(gui2->hasKeyboardFocus())
	//    {
	//       return;
	//    }
	// cout << "keyPressed " << key << endl;
	switch (key)
	{
		//356 LEFT
		//357 UP
		//358 RIGHT
		//359 DOWN
	case 356:
		contourSelected--;
		break;
	case 358:
		contourSelected++;
		break;
	case 'g':
		gui->toggleVisible();
		break;
	case 'p':
		isProductive=!isProductive;
		break;
	case 'o':
		isCtrlComposite=!isCtrlComposite;
		break;
	case 'i':
		isInteractive=!isInteractive;
		isContours=!isContours;
		movingShape.clear(); //cleanup leftover from interaction
		break;
	case 'r':
		isRaining=!isRaining;
		break;
	case 'b':
		isExplosion=true;
		if (contoursOnscreen>0) { explodeShape(); }
		break;

	case 'c':
		explodingShapeLine.clear();
		movingShapeLine.clear();
		movingShape.clear();
		polyShapes.clear();
		circles.clear();
		boxes.clear();
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
