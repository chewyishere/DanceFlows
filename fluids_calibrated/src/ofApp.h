#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "ofxSecondWindow.h"
#include "ofxKinectProjectorToolkit.h"
#include "ofxFluid.h"
#include "ofxUI.h"


#define PROJECTOR_RESOLUTION_X 1280
#define PROJECTOR_RESOLUTION_Y 800
#define CALIBRATION_FILE "/Users/Gene/Desktop/calibration.xml"


class ofApp : public ofBaseApp{
    
public:
    
	void setup();
	void update();
	void draw();
    void exit();
    
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
    
    void userEvent(ofxOpenNIUserEvent & event);
    
	ofxOpenNI kinect;
    
    ofxKinectProjectorToolkit kpt;
    ofxSecondWindow projector;
    
    
    vector<vector<ofVec2f> > prevJoints;
    int maxUsers = 3;
    
    ofxFluid fluid;
    
    ofxUICanvas *gui;
    float dissipation, velDissipation;
    float displacement;
    float strength;
    float gravityX, gravityY;

    
};
