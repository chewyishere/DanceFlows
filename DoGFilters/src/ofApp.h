//
//  ofApp.h
//  ofxFilterLibraryExample
//
//  Created by Matthew Fargo on 2014/06/25.
//
//
//  This is an example of how to use all the filters in this library.
//  They should be as easy as coffee filters, but not as easy as
//  cigarette filters.

#pragma once

#include "ofxFilterLibrary.h"
#include "LineDrawingAlphaFilter.h"
#include "SketchDoGFilter.h"
#include "ofxKinect.h"

typedef enum {
    DOG_MODE_ANIME,
    DOG_MODE_TAKE_ON_ME
} DogMode;

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
	    //void exit();
        void setMode(DogMode mode);
    
		void keyPressed(int key);
        ofImage                     myImg;
    int w;
    int h;

    private:
        DogMode                     _mode;
        ofxKinect                 _video;
        //ofVideoGrabber              _video;
    
        FilterChain *               _filters;
        LineDrawingAlphaFilter *    _alphaFilter;
        ofFbo                       _dogFbo;
        DoGFilter *                 _dogFilter;
        SketchDoGFilter *           _sketchDogFilter;

};
