GrayPlay
========

WHAT? interactive projection mapping using kinect and projector done in openframeworks on windows for "buy nothing day" theatre play (https://www.facebook.com/ziuaincarenusecumparanimic)
(...)

HOW? started from the example from https://github.com/genekogan/ofxKinectProjectorToolkit (thank you genekogan!) using ofxkinect's freelibnet drivers (.inf) files fom somewhere deep in the ofxKinect folders
(...)

DEPENDENCES? addons needed in the ofx's \addons folder (so far)

ofxOpenCv
ofxCv
ofxKinect
ofxSecondWindow
ofxKinectProjectorToolkit
ofxUI
ofURLFileLoader

EXCUSES? because I am totally not a programmer, and I am using github, I pointed the repository to the openframeworks' myApp folder, where usually I generate folders using the projectGenerator.

calibrate\ 
          - basically the same calibraton process from here - https://github.com/genekogan/ofxKinectProjectorToolkit
          - hacked a variable to allow the actual calibration to start (TODO fork and submit pull?)
          - will produce a xml.
play\
          - based on  the example from here - https://github.com/genekogan/ofxKinectProjectorToolkit/tree/master/example_bodyMapping
          - will load the calibration xml
          - first window a control panel for the kinect/projector (gui)
              -save presets (f1-f4) / load presets (1-4)
              -show the mapping vs actual in the 4th square
kinect\
          - the default ofxKinect demo, I generally use it to figure out if the kinect does what it should (and also check the capabilities)
