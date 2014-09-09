GrayPlay
========
This project is an attempt to create a simple projection mapping implementation using kinect and a projector, to be used for a theater [play](https://www.facebook.com/ziuaincarenusecumparanimic).

Development
-----------
I'm learning by doing both programming and GitHub. I started this project by using genekogan's [ofxKinectProjectorToolkit](https://github.com/genekogan/ofxKinectProjectorToolkit) 's example (calibration and bodyMapping) to which I am adding things - and see where it leads. 

Installation
------------
Clone this to the apps folder of your everyday openframework. <br>
Plug kinect in, update drivers using (.inf) files fom addons\ofxKinect\libs\libfreenect\platform\windows\inf<br>

####calibrate
- basically the same calibraton process from [ofxKinectProjectorToolkit](https://github.com/genekogan/ofxKinectProjectorToolkit)
- will produce a xml.

####play
- based on the example from [ofxKinectProjectorToolkit](https://github.com/genekogan/ofxKinectProjectorToolkit/tree/master/example_bodyMapping)
- will load the calibration xml
- will have a control pannel on the first screen
- will produce the projected composite on a 2nd screen

####playbox
- dev for bringing box2d in the picture

####kinect
- the default ofxKinect demo, I generally use it to figure out if the kinect does what it should (and also check the capabilities)

Usage
-----
Start calibration app and follow it up by moving the chessboard around and in depth until mapped. Copy the produced xml file to the play app. Start play app, use the first screen as the control pannel and the second screen as the life performance. 

Key | Function
:--- | :---
F1-F4 | Save current parameters to a file
1-4 | Load parameters from file
p | Hide elements in the 1st window (production mode)

Dependencies
------------
Addons for openframeworks.
- ofxXmlSettings
- ofxOpenCv
- ofxCv
- ofxKinect
- ofxSecondWindow
- ofxKinectProjectorToolkit
- ofxUI
- ofURLFileLoader
and possibly
- ofxBox2d

Compatibility
------------
Tested with 0.8.3 (win8, win8.1)

Known issues
------------
I'm learning so this will be very messy.

Version history
---------------

### Version 0.1.1 (2014-09-10):
Some copy/paste programming to allow 'presets' to show as proof of concept (currently just color change). 
 - Formatted README
 - Added a 4th screen in the control pannel of play, showing the life performance in control pannel and the real-life kinect
 - Added presets (f1-f4,1-4)
 - Cloned play to playbox and added

### Version 0.1 (2014-09-01):
First version, mostly clones and succesful compliles using visual studio.
 - Cloned calibration and made it work (adding testing=false)
 - Cloned body mapping example 
 - Cloned kinect default example from ofxKinect to be used as a default hw check if kinect works.
