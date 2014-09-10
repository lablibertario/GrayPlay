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

Usage
-----
Start calibration app and follow it up by moving the chessboard around and in depth until mapped. 

####calibrate
- basically the same calibraton process from [ofxKinectProjectorToolkit](https://github.com/genekogan/ofxKinectProjectorToolkit)
- will produce a xml.

Copy the produced xml file to the play* app(s).

####play
- based on the example from [ofxKinectProjectorToolkit](https://github.com/genekogan/ofxKinectProjectorToolkit/tree/master/example_bodyMapping)
- loads the calibration xml
- control panel on the first screen
- projected composite on a 2nd screen

Key | Function
:--- | :---
F1-F4 | Save current parameters to a file
1-4 | Load parameters from file
p | Hide elements in the 1st window (production mode)

####playbox
- uses ofxBox2d
- loads calibration xml
- control panel first screen
- projection screen 

Key | Function
:--- | :---
F1-F4 | Save current parameters to a file
1-4 | Load parameters from file
p | Hide elements in the 1st window (production mode)
o | Switch between the Kinect views on the control panel
b | "Breaks" the last shape into 2dBox physics triangles
c | Clears all 2dbox stuff
(not assigned)  | interface "interactive" flag makes the last shape interactive in 2dBox world
(not assigned)  | interface "raining" flag makes 2dbox cicles and boxes fall from the sky

####kinect
- the default ofxKinect demo, I generally use it to figure out if the kinect does what it should (and also check the capabilities)


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
- ofxBox2d (used only for boxplay)

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
