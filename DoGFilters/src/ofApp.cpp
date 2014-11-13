#include "ofApp.h"

void ofApp::setup(){
    ofDisableArbTex();
    myImg.loadImage("01.jpeg");
   // _video.initGrabber(1280, 720);
   w = 1280;
   h =720;
    ofSetLogLevel(OF_LOG_VERBOSE);
	
	_video.setRegistration(true);
    
	_video.init();
    _video.open();

    
    _filters = new FilterChain(w,h, "Suisaiga");
    _filters->addFilter(new KuwaharaFilter(1)); //9
    _filters->addFilter(new BilateralFilter(w,h, 6.f, 4.f));
    
    _alphaFilter = new LineDrawingAlphaFilter();
    _dogFilter = new DoGFilter(w,h,11, 1.7, 8.5, 0.973, 4);
    _sketchDogFilter = new SketchDoGFilter(w,h, 11, 1.7, 8.5, 0.973, 4);
    setMode(DOG_MODE_ANIME);
    _dogFbo.allocate(w,h);
}

void ofApp::update(){
    _video.update();

}

void ofApp::draw(){
    ofBackground(255);
    ofPushMatrix();
    ofScale(-1, 1);
    ofTranslate(-ofGetWidth(), 0);
    ofEnableAlphaBlending();
    
    ofSetColor(255);
    
    if (_mode==DOG_MODE_ANIME) {
        _filters->begin();
        _video.draw(0,0,w,h);
        _filters->end();
    }
    
    _dogFbo.begin();

    if (_mode==DOG_MODE_ANIME) _dogFilter->begin();
    else _sketchDogFilter->begin();
    _video.draw(0, 0,w,h);
    if (_mode==DOG_MODE_ANIME) _dogFilter->end();
    else _sketchDogFilter->end();
    _dogFbo.end();

    _alphaFilter->begin();
    _dogFbo.draw(0, 0,w,h);
    _alphaFilter->end();

    ofPopMatrix();
}

void ofApp::setMode(DogMode mode) {
    _mode = mode;
    switch (mode) {
        case DOG_MODE_ANIME:
            _dogFilter->setBlack(11);
            _dogFilter->setSigma(1.7);
            _dogFilter->setSigma3(8.5);
            _dogFilter->setTau(0.973);
            _dogFilter->setHalfWidth(4);
            _dogFilter->setSketchiness(ofVec2f(1.0, 1.0));

            break;
        case DOG_MODE_TAKE_ON_ME:
            
            _sketchDogFilter->setBlack(56);
            _sketchDogFilter->setSigma(1.36);
            _sketchDogFilter->setSigma3(0.01);
            _sketchDogFilter->setTau(0.897);
            _sketchDogFilter->setHalfWidth(4);
            _sketchDogFilter->setSketchiness(ofVec2f(5.3, 0.0));
                                 
            break;
            
    }
}

void ofApp::keyPressed(int key){
    if (key==' ') {
        if (_mode==DOG_MODE_ANIME) setMode(DOG_MODE_TAKE_ON_ME);
        else setMode(DOG_MODE_ANIME);
    }
    else if (key=='f') ofToggleFullscreen();
}
