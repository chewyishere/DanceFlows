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
    
    numberBlend = 2;
    
    //   ************* Kinect Stuff *************
    
    ofSetLogLevel(OF_LOG_VERBOSE);
	
	kinect.setRegistration(true);
    
	kinect.init();
    kinect.open();
	
	nearThreshold = 230;
	farThreshold = 70;
	bThreshWithOpenCV = true;
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);


}

void testApp::setupGui() {
	
	gui.setup("settings");
	gui.setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
	gui.setDefaultFillColor(ofColor(160, 160, 160, 160));
	gui.add(guiFPS.set("FPS", 0, 0, 60));
	gui.add(doFullScreen.set("fullscreen (F)", true));
	doFullScreen.addListener(this, &testApp::setFullScreen);
	gui.add(toggleGuiDraw.set("show gui (G)", false));
	gui.add(doFlipCamera.set("flip camera (C)", true));
	numVisualisationModes = 3;
	gui.add(visualisationMode.set("visualisation mode", 0, 0, numVisualisationModes - 1));
	gui.add(visualisationName.set("MODE", "draw name"));
	visualisationModeTitles = new string[numVisualisationModes];
    visualisationModeTitles[0] = "flows";
    visualisationModeTitles[1] = "camera";

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

	toggleGuiDraw = false;
    
}
//--------------------------------------------------------------
void testApp::update(){
    
    kinect.update();
    
	deltaTime = ofGetElapsedTimef() - lastTime;
	lastTime = ofGetElapsedTimef();
    
    
    if( numberBlend == 1)blendMode = OF_BLENDMODE_ADD;
    else if( numberBlend == 2)blendMode = OF_BLENDMODE_SCREEN;
    else if( numberBlend == 3)blendMode = OF_BLENDMODE_SUBTRACT;
    else if( numberBlend == 4)blendMode = OF_BLENDMODE_MULTIPLY;
    else if( numberBlend == 5)blendMode = OF_BLENDMODE_DISABLED;

    
}


//--------------------------------------------------------------
void testApp::draw(){
    ofClear(0);
    
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    cameraFbo.begin();
    if (doFlipCamera)
        kinect.draw(cameraFbo.getWidth(), 0, -cameraFbo.getWidth(), cameraFbo.getHeight());  // Flip Horizontal
    else
        kinect.draw(0, 0, cameraFbo.getWidth(), cameraFbo.getHeight());
    cameraFbo.end();
    ofPopStyle();
    
    opticalFlow.setSource(cameraFbo.getTextureReference());
    opticalFlow.update(deltaTime);
    
    velocityMask.setDensity(cameraFbo.getTextureReference());
    velocityMask.setVelocity(opticalFlow.getOpticalFlow());
    velocityMask.update();

    fluid.addVelocity(opticalFlow.getOpticalFlowDecay());
    fluid.addDensity(velocityMask.getColorMask());
    fluid.addTemperature(velocityMask.getLuminanceMask());

    for (int i=0; i<numDrawForces; i++) {
        flexDrawForces[i].update();
        if (flexDrawForces[i].didChange()) {
            // if a force is constant multiply by deltaTime
            float strength = flexDrawForces[i].getStrength();
            if (!flexDrawForces[i].getIsTemporary())
                strength *=deltaTime;
                switch (flexDrawForces[i].getType()) {
                    case flowTools::FT_DENSITY:
                        fluid.addDensity(flexDrawForces[i].getTextureReference(), strength);
                        break;
                    case flowTools::FT_VELOCITY:
                        fluid.addVelocity(flexDrawForces[i].getTextureReference(), strength);
                        particleFlow.addFlowVelocity(flexDrawForces[i].getTextureReference(), strength);
                        break;
                    case flowTools::FT_TEMPERATURE:
                        fluid.addTemperature(flexDrawForces[i].getTextureReference(), strength);
                        break;
                    case flowTools::FT_PRESSURE:
                        fluid.addPressure(flexDrawForces[i].getTextureReference(), strength);
                        break;
                    case flowTools::FT_OBSTACLE:
                        fluid.addTempObstacle(flexDrawForces[i].getTextureReference());
                    default:
                        break;
                }
        }
    }
            
    
    fluid.update();
    
	if (particleFlow.isActive()) {
		particleFlow.setSpeed(fluid.getSpeed());
		particleFlow.setCellSize(fluid.getCellSize());
		particleFlow.addFlowVelocity(opticalFlow.getOpticalFlow());
		particleFlow.addFluidVelocity(fluid.getVelocity());
		particleFlow.setObstacle(fluid.getObstacle());
	}
	particleFlow.update();
   
    ofClear(0,0);
    
    switch(visualisationMode.get()) {
            
        case 2: // Fluid Compos
            
            ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_DISABLED);
            cameraFbo.begin();
            
            if (doFlipCamera)
                kinect.draw(cameraFbo.getWidth(), 0, -ofGetWindowWidth(), ofGetWindowHeight());
            else
                kinect.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
            
            cameraFbo.end();
        
            cameraFbo.draw(0,0, ofGetWindowWidth(), ofGetWindowHeight());
			
            ofEnableBlendMode(blendMode);
			fluid.draw(0, 0,ofGetWindowWidth(),ofGetWindowHeight());
			
            if (particleFlow.isActive())
				particleFlow.draw(0, 0,ofGetWindowWidth(), ofGetWindowHeight());
            
            ofPopStyle();
			break;
            
        case 1: // Fluid Color
            
            ofPushStyle();
            ofEnableBlendMode(OF_BLENDMODE_DISABLED);
            cameraFbo.begin();
            
            if (doFlipCamera)
                kinect.draw(cameraFbo.getWidth(), 0, -ofGetWindowWidth(), ofGetWindowHeight());
            else
                kinect.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
            
            cameraFbo.end();
            
            ofBackgroundGradient(ofColor(abs(sin(ofGetElapsedTimef()*0.05)*255),
                                         abs(sin(ofGetElapsedTimef()*0.03)*255),
                                         abs(sin(ofGetElapsedTimef()*0.08)*255)),
                                 ofColor(0));
            
            ofEnableBlendMode(blendMode);
            
            opticalFlow.setStrength(abs(sin(ofGetElapsedTimef()*0.03)*60));
            fluid.setCellSize(abs(sin(ofGetElapsedTimef()*0.008)*2));
            fluid.setNumJacobiIterations(100);
            fluid.setViscosity(abs(sin(ofGetElapsedTimef()*0.05)*1));
            fluid.setVorticity(abs(sin(ofGetElapsedTimef()*0.01)*1));
            fluid.setDissipation(0.02);
            fluid.setSpeed(abs(sin(ofGetElapsedTimef()*0.03)*60));
            velocityMask.setSaturation(abs(sin(ofGetElapsedTimef()*0.02)*5));
            particleFlow.activate(false);

            
            //velocityMask.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
			fluid.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
	
			if (particleFlow.isActive())
				particleFlow.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
            
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
        case '1':
            visualisationMode.set(1);
            break;
        case '2':
            visualisationMode.set(2);
            break;
        case '3':
            visualisationMode.set(3);
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

        case 'z':
            numberBlend = 1;
            break;
        case 'x':
            numberBlend = 2;
            break;
        case 'c':
            numberBlend = 3;
            break;
        case 'v':
            numberBlend = 4;
            break;
        case 'b':
            numberBlend = 5;
            break;
            
    }

}

