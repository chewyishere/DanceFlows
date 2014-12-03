#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    //kinect.setup();
    kinect.setupFromONI("/Users/Gene/Code/openFrameworks/templates/Kinect/openni_oniRecording/bin/data/alecsroom.oni");
    kinect.addImageGenerator();
    kinect.addDepthGenerator();
    kinect.setRegister(true);
    kinect.setMirror(true);
    kinect.addUserGenerator();
    kinect.setMaxNumUsers(2);
    kinect.start();
    
    // set properties for all user masks and point clouds
    //kinect.setUseMaskPixelsAllUsers(true); // if you just want pixels, use this set to true
    kinect.setUseMaskTextureAllUsers(true); // this turns on mask pixels internally AND creates mask textures efficiently
    kinect.setUsePointCloudsAllUsers(true);
    kinect.setPointCloudDrawSizeAllUsers(2); // size of each 'point' in the point cloud
    kinect.setPointCloudResolutionAllUsers(2); // resolution of the mesh created for the point cloud eg., this will use every second depth pixel
    
    
    //projector.setup("main", ofGetScreenWidth(), 0, PROJECTOR_RESOLUTION_X, PROJECTOR_RESOLUTION_Y, false);
    projector.setup("main", ofGetScreenWidth()-500, 0, PROJECTOR_RESOLUTION_X, PROJECTOR_RESOLUTION_Y, false);
    
    kpt.loadCalibration(CALIBRATION_FILE);
    
    
    
    // hold previous joints
    for (int i=0; i<maxUsers; i++) {
        vector<ofVec2f> newVec;
        newVec.resize(15);
        prevJoints.push_back(newVec);
    }
    
    
    
    // Initial Allocation
    fluid.allocate(projector.getWidth(), projector.getHeight(), 0.5);
    
    // Seting the gravity set up & injecting the background image
    fluid.dissipation = 0.99;
    fluid.velocityDissipation = 0.99;
    
    fluid.setGravity(ofVec2f(0.0,0.0));
    //    fluid.setGravity(ofVec2f(0.0,0.0098));
    
    //  Set obstacle
    fluid.begin();
    ofSetColor(0,0);
    ofSetColor(255);
    ofCircle(projector.getWidth()*0.5, projector.getHeight()*0.35, 40);
    fluid.end();
    fluid.setUseObstacles(false);
    
    // Optional: Adding constant forces
    fluid.addConstantForce(ofPoint(projector.getWidth()*0.5, projector.getHeight()*0.85),
                           ofPoint(0,-2),
                           ofFloatColor(0.5,0.1,0.0),
                           10.f);
    
    
    // default variables
    dissipation = 0.99;
    velDissipation = 0.99;
    gravityX = 0.0;
    gravityY = 0.0;
    displacement = 10;
    strength = 5.0;
    
    
    // setup gui
    gui = new ofxUICanvas("fluids");
    gui->setPosition(500, 20);
    gui->addSlider("dissipation", 0, 1, &dissipation);
    gui->addSlider("vel dissipation", 0, 1, &velDissipation);
    gui->addSlider("displacement", 0, 100, &displacement);
    gui->addSlider("strength", 0, 100, &strength);
    gui->addSlider("gravityX", -0.02, 0.02, &gravityX);
    gui->addSlider("gravityY", -0.02, 0.02, &gravityY);
    gui->autoSizeToFitWidgets();
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    
    // Adding temporal Force
    int numUsers = kinect.getNumTrackedUsers();
    for (int i = 0; i < min(maxUsers, numUsers); i++){
        ofxOpenNIUser & user = kinect.getTrackedUser(i);
        
        for (int j=0; j<user.getNumJoints(); j++) {
            ofPoint joint = user.getJoint((Joint &) j).getWorldPosition();
            joint.x *= -1.0f;
            joint.y *= -1.0f;
            
            ofVec2f projectedJoint = kpt.getProjectedPoint(joint);
            projectedJoint.x *= projector.getWidth();
            projectedJoint.y *= projector.getHeight();
            
            
            ofPoint m = ofPoint(projectedJoint.x, projectedJoint.y);
            ofPoint d = (m - prevJoints[i][j])*10.0;
            ofPoint c = ofPoint(640*0.5, 480*0.5) - m;
            c.normalize();
            fluid.addTemporalForce(m,
                                   d,
                                   ofFloatColor(c.x,c.y,0.5)*sin(ofGetElapsedTimef()),
                                   strength);
            
            prevJoints[i][j] = projectedJoint;
        }
    }
    
    //fluid.setGravity(ofVec2f(gravityX, gravityY));
    fluid.dissipation = dissipation;
    fluid.velocityDissipation = velDissipation;
    
    fluid.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255);
    ofPushMatrix();
    kinect.drawDebug();
    ofPopMatrix();
    ofPushMatrix();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    int numUsers = kinect.getNumTrackedUsers();
    for (int i = 0; i < numUsers; i++){
        ofxOpenNIUser & user = kinect.getTrackedUser(i);
        ofPushMatrix();
        user.drawMask();
        ofPopMatrix();
    }
    ofDisableBlendMode();
    ofPopMatrix();
    

    // PROJECTOR VIEW
    
    projector.begin();

    ofBackgroundGradient(ofColor::gray, ofColor::black, OF_GRADIENT_LINEAR);
    fluid.draw();
    
    projector.end();
    
}

//--------------------------------------------------------------
void ofApp::userEvent(ofxOpenNIUserEvent & event){
    // show user event messages in the console
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user" << event.id << "from device" << event.deviceID;
}

//--------------------------------------------------------------
void ofApp::exit(){
    kinect.stop();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}