#include "ofApp.h"

//--------------------------------------------------------------
void testApp::setup(){

     ofSetVerticalSync(false);
    
	drawWidth = 1024;
	drawHeight = 768;
	// process all but the density on 16th resolution
    
	flowWidth = drawWidth/4;
	flowHeight = drawHeight/4;
	
	// Flow & Mask
	opticalFlow.setup(flowWidth, flowHeight);
	velocityMask.setup(drawWidth, drawHeight);
	
	// Fluid
    
#ifdef USE_FASTER_INTERNAL_FORMATS
	fluid.setup(flowWidth, flowHeight, drawWidth, drawHeight, true);
#else
	fluid.setup(flowWidth, flowHeight, drawWidth, drawHeight, false);
#endif
	
	// Particles
	particleFlow.setup(flowWidth, flowHeight, drawWidth, drawHeight);
	
	// Visualisation
	displayScalar.allocate(flowWidth, flowHeight);
	velocityField.allocate(flowWidth / 4, flowHeight / 4);
	temperatureField.allocate(flowWidth / 4, flowHeight / 4);
	
	// Draw Forces
	numDrawForces = 6;
	flexDrawForces = new flowTools::ftDrawForce[numDrawForces];
	flexDrawForces[0].setup(drawWidth, drawHeight, flowTools::FT_DENSITY, true);
	flexDrawForces[0].setName("draw full res");
	flexDrawForces[1].setup(flowWidth, flowHeight, flowTools::FT_VELOCITY, true);
	flexDrawForces[1].setName("draw flow res 1");
	flexDrawForces[2].setup(flowWidth, flowHeight, flowTools::FT_TEMPERATURE, true);
	flexDrawForces[2].setName("draw flow res 2");
	flexDrawForces[3].setup(drawWidth, drawHeight, flowTools::FT_DENSITY, false);
	flexDrawForces[3].setName("draw full res");
	flexDrawForces[4].setup(flowWidth, flowHeight, flowTools::FT_VELOCITY, false);
	flexDrawForces[4].setName("draw flow res 1");
	flexDrawForces[5].setup(flowWidth, flowHeight, flowTools::FT_TEMPERATURE, false);
	flexDrawForces[5].setName("draw flow res 2");
	
	// Camera
	cameraFbo.allocate(drawWidth, drawHeight);
	cameraFbo.begin(); ofClear(0); cameraFbo.end();
    
	setupGui();
	
	lastTime = ofGetElapsedTimef();
	lastMouse.set(0,0);
    
    
    //   ************* Kinect Stuff *************
    
    ofSetLogLevel(OF_LOG_VERBOSE);
	
	kinect.setRegistration(true);
    
	kinect.init();
    kinect.open();
	
	
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	nearThreshold = 230;
	farThreshold = 70;
	bThreshWithOpenCV = true;
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 30;
	kinect.setCameraTiltAngle(angle);


}

void testApp::setupGui() {
	
	gui.setup("settings");
	gui.setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
	gui.setDefaultFillColor(ofColor(160, 160, 160, 160));
	gui.add(guiFPS.set("FPS", 0, 0, 60));
	gui.add(doFullScreen.set("fullscreen (F)", false));
	doFullScreen.addListener(this, &testApp::setFullScreen);
	gui.add(toggleGuiDraw.set("show gui (G)", false));
	gui.add(doFlipCamera.set("flip camera (C)", true));
	numVisualisationModes = 10;
	gui.add(visualisationMode.set("visualisation mode", 0, 0, numVisualisationModes - 1));
	gui.add(visualisationName.set("MODE", "draw name"));
	
	visualisationModeTitles = new string[numVisualisationModes];
    visualisationModeTitles[0] = "Source         (0)";
    visualisationModeTitles[1] = "Fld Composite  (1)";
	visualisationModeTitles[2] = "Fluid Color    (2)";
	visualisationModeTitles[3] = "Flow Mask      (3)";
    visualisationModeTitles[4] = "Point Cloud    (4)";
    visualisationModeTitles[5] = "Lines          (5)";
    
	int guiColorSwitch = 0;
	ofColor guiHeaderColor[2];
	guiHeaderColor[0].set(160, 160, 80, 200);
	guiHeaderColor[1].set(80, 160, 160, 200);
	ofColor guiFillColor[2];
	guiFillColor[0].set(160, 160, 80, 200);
	guiFillColor[1].set(80, 160, 160, 200);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(opticalFlow.parameters);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(velocityMask.parameters);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(fluid.parameters);
    
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(particleFlow.parameters);
	
	visualisationParameters.setName("visualisation");
	visualisationParameters.add(showScalar.set("show scalar", true));
	visualisationParameters.add(showField.set("show field", true));
	visualisationParameters.add(displayScalarScale.set("display scalar scale", 0.25, 0.05, 0.5));
	displayScalarScale.addListener(this, &testApp::setDisplayScalarScale);
	visualisationParameters.add(velocityFieldArrowScale.set("arrow scale", 0.6, 0.2, 1));
	velocityFieldArrowScale.addListener(this, &testApp::setVelocityFieldArrowScale);
	visualisationParameters.add(temperatureFieldBarScale.set("temperature scale", 0.25, 0.05, 0.5));
	temperatureFieldBarScale.addListener(this, &testApp::setTemperatureFieldBarScale);
	visualisationParameters.add(visualisationLineSmooth.set("line smooth", false));
	visualisationLineSmooth.addListener(this, &testApp::setVisualisationLineSmooth);
    
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(visualisationParameters);
	
	leftButtonParameters.setName("mouse left button");
	for (int i=0; i<3; i++) {
		leftButtonParameters.add(flexDrawForces[i].parameters);
	}
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(leftButtonParameters);
	
	rightButtonParameters.setName("mouse right button");
	for (int i=3; i<6; i++) {
		rightButtonParameters.add(flexDrawForces[i].parameters);
	}
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(rightButtonParameters);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(doResetDrawForces.set("reset draw forces (D)", false));
	doResetDrawForces.addListener(this,  &testApp::resetDrawForces);
    
	gui.loadFromFile("settings.xml");
	gui.minimizeAll();
	
	toggleGuiDraw = true;
    
    
}
//--------------------------------------------------------------
void testApp::update(){
    
    kinect.update();
	
	if(kinect.isFrameNew()) {
		
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
		
		if(bThreshWithOpenCV) {
			grayThreshNear = grayImage;
			grayThreshFar = grayImage;
			grayThreshNear.threshold(nearThreshold, true);
			grayThreshFar.threshold(farThreshold);
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
		} else {
			
            unsigned char * pix = grayImage.getPixels();
			
			int numPixels = grayImage.getWidth() * grayImage.getHeight();
			for(int i = 0; i < numPixels; i++) {
				if(pix[i] < nearThreshold && pix[i] > farThreshold) {
					pix[i] = 255;
				} else {
					pix[i] = 0;
				}
			}
		}
		
		// update the cv images
		grayImage.flagImageChanged();
		
		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/3, 20, false);
	}
    
	deltaTime = ofGetElapsedTimef() - lastTime;
	lastTime = ofGetElapsedTimef();
    
}


//--------------------------------------------------------------
void testApp::draw(){

    ofClear(0);
    
    opticalFlow.setSource(cameraFbo.getTextureReference());
    opticalFlow.update(deltaTime);
    
    velocityMask.setDensity(cameraFbo.getTextureReference());
    velocityMask.setVelocity(opticalFlow.getOpticalFlow());
    velocityMask.update();
    
    
    fluid.addVelocity(opticalFlow.getOpticalFlowDecay());
    fluid.addDensity(velocityMask.getColorMask());
    fluid.addTemperature(velocityMask.getLuminanceMask());
	
    fluid.update();
    
	if (particleFlow.isActive()) {
		particleFlow.setSpeed(fluid.getSpeed());
		particleFlow.setCellSize(fluid.getCellSize());
		particleFlow.addFlowVelocity(opticalFlow.getOpticalFlow());
		particleFlow.addFluidVelocity(fluid.getVelocity());
		particleFlow.setObstacle(fluid.getObstacle());
	}
	particleFlow.update();
	
    kinect.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
    cameraFbo.draw(0,0, ofGetWindowWidth(),ofGetWindowHeight());
    
    ofClear(0,0);
    
    switch(visualisationMode.get()) {
            
        case 0: // Source
			cameraFbo.draw(0,0, ofGetWindowWidth(), ofGetWindowHeight());
			break;
            
        case 1: // Fluid Compos
            
            ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_DISABLED);
            cameraFbo.begin();
            
            if (doFlipCamera)
                kinect.draw(cameraFbo.getWidth(), 0, -ofGetWindowWidth(), ofGetWindowHeight());
            else
                kinect.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
            
            cameraFbo.end();
        
            cameraFbo.draw(0,0, ofGetWindowWidth(), ofGetWindowHeight());
			
			ofEnableBlendMode(OF_BLENDMODE_ADD);
			fluid.draw(0, 0,ofGetWindowWidth(),ofGetWindowHeight());
			
            if (particleFlow.isActive())
				particleFlow.draw(0, 0,ofGetWindowWidth(), ofGetWindowHeight());
            
            ofPopStyle();
			break;
            
        case 2: // Fluid Color
            
            ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_DISABLED);
            cameraFbo.begin();
            
            if (doFlipCamera)
                kinect.draw(cameraFbo.getWidth(), 0, -ofGetWindowWidth(), ofGetWindowHeight());
            else
                kinect.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
            
            cameraFbo.end();
            
            ofEnableBlendMode(OF_BLENDMODE_SCREEN);
			fluid.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	
			if (particleFlow.isActive())
				particleFlow.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
            
            ofPopStyle();
			break;
            
        case 3: // velocityMask
            
            
            ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_DISABLED);
            cameraFbo.begin();
            if (doFlipCamera)
                kinect.draw(cameraFbo.getWidth(), 0, -ofGetWindowWidth(), ofGetWindowHeight());
            else
                kinect.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
            
            cameraFbo.end();
            
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            velocityMask.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
            ofPopStyle();
            break;
            
        case 4: // Point Cloud
            
            ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_DISABLED);
            easyCam.begin();
            cameraFbo.begin();
            kinect.draw(cameraFbo.getWidth(), 0, -ofGetWindowWidth(), ofGetWindowHeight());
            cameraFbo.end();
            
            ofEnableBlendMode(OF_BLENDMODE_ADD);
	        particleFlow.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
            drawPointCloud();
            easyCam.end();
            
            ofPopStyle();
            break;
            
            
        case 5: // Point Lines
            ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_DISABLED);
            easyCam.begin();
            cameraFbo.begin();
            kinect.draw(cameraFbo.getWidth(), 0, -ofGetWindowWidth(), ofGetWindowHeight());
            cameraFbo.end();
            
			ofEnableBlendMode(OF_BLENDMODE_ADD);
            drawLines();
            easyCam.end();
            
            ofPopStyle();
            break;

    }
    
    if (toggleGuiDraw) {
		guiFPS = ofGetFrameRate();
		if (visualisationMode.get() >= numVisualisationModes)
			visualisationMode.set(numVisualisationModes-1);
		visualisationName.set(visualisationModeTitles[visualisationMode.get()]);
		gui.draw();
	}

    
}
//--------------------------------------------------------------
void testApp::drawPointCloud() {
    
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				mesh.addColor(kinect.getColorAt(x,y));
				mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
			}
		}
	}
	glPointSize(3);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards'
	ofScale(-1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	ofEnableDepthTest();
    mesh.draw();
	ofDisableDepthTest();
	ofPopMatrix();
}

//--------------------------------------------------------------

void testApp::drawLines() {
    
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);
	int step = 2;
    
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
                
                ofColor realColor = ofColor(kinect.getColorAt(x,y));
                // how to add random?
				mesh.addColor(realColor);
                mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
			}
		}
	}
	glLineWidth(2);
    ofPushMatrix();
    ofScale(-1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	ofEnableDepthTest();
    //mesh.draw();
    mesh.drawFaces();
	ofDisableDepthTest();
	ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}
//--------------------------------------------------------------

void testApp::keyPressed(int key){
    
    switch (key) {
            
        case 'G' :
            toggleGuiDraw = !toggleGuiDraw;
            break;
        case 'R':
            fluid.reset();
            break;
        case 'D':
            doResetDrawForces.set(true);
            break;
        case '0':
            visualisationMode.set(0);
            break;
        case '1':
            ofEnableArbTex();

            visualisationMode.set(1);
            break;
        case '2':
            ofEnableArbTex();

            visualisationMode.set(2);
            break;
        case '3':
            visualisationMode.set(3);
            break;
        case '4':
            visualisationMode.set(4);
            break;
        case '5':
            visualisationMode.set(5);
            break;
        case '6':
            visualisationMode.set(6);
            break;
        case '7':
            visualisationMode.set(7);
            break;

		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
			
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
            
			         
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}


}

