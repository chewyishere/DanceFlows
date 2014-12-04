#include "ofApp.h"

void ofApp::setup(){
    ofDisableArbTex();
    ofSetVerticalSync(false);
   // myImg.loadImage("img/04.jpg");
    ofSetLogLevel(OF_LOG_VERBOSE);
	
	_video.setRegistration(true);
    
	_video.init();
    _video.open();
    
   	KWidth = 1280;
	KHeight = 720;
    
    // ++++++++++++++ filters ++++++++++++++++++

    _currentFilter = 0;

    // Basic filter examples
    
    _filters.push_back(new KuwaharaFilter());
    _filters.push_back(new SobelEdgeDetectionFilter(KWidth, KHeight));
    _filters.push_back(new BilateralFilter(KWidth, KHeight));
    _filters.push_back(new SketchFilter(KWidth, KHeight));
    _filters.push_back(new DilationFilter(KWidth, KHeight));
    _filters.push_back(new PerlinPixellationFilter(KWidth, KHeight));
    _filters.push_back(new XYDerivativeFilter(KWidth, KHeight));
    _filters.push_back(new ZoomBlurFilter());
    _filters.push_back(new EmbossFilter(KWidth, KHeight, 2.f));
    _filters.push_back(new SmoothToonFilter(KWidth, KHeight));
    _filters.push_back(new TiltShiftFilter(_video.getTextureReference()));
    _filters.push_back(new VoronoiFilter(_video.getTextureReference()));    
    _filters.push_back(new DoGFilter(KWidth, KHeight, 11, 1.61, 5.01, 0.977, 8));
    _filters.push_back(new CGAColorspaceFilter());
    _filters.push_back(new ErosionFilter(KWidth, KHeight));
    _filters.push_back(new LookupFilter(KWidth, KHeight, "img/lookup_amatorka.png"));
    _filters.push_back(new LookupFilter(KWidth, KHeight, "img/lookup_miss_etikate.png"));
    _filters.push_back(new LookupFilter(KWidth, KHeight, "img/lookup_soft_elegance_1.png"));
    _filters.push_back(new VignetteFilter());
    _filters.push_back(new PosterizeFilter(8));
    _filters.push_back(new LaplacianFilter(KWidth, KHeight, ofVec2f(1, 1)));
    _filters.push_back(new PixelateFilter(KWidth, KHeight));
    _filters.push_back(new HarrisCornerDetectionFilter(_video.getTextureReference()));
    _filters.push_back(new MotionDetectionFilter(_video.getTextureReference()));
    _filters.push_back(new LowPassFilter(KWidth, KHeight, 0.9));
    _filters.push_back(new DisplacementFilter("img/mandel.jpg", KWidth, KHeight, 25.f));
    _filters.push_back(new PoissonBlendFilter("img/wes.jpg", KWidth, KHeight, 2.0));
    _filters.push_back(new DisplacementFilter("img/glass/3.jpg", KWidth, KHeight, 40.0));

    
    // here's how you use Convolution filters
    
    Abstract3x3ConvolutionFilter * convolutionFilter1 = new Abstract3x3ConvolutionFilter(KWidth, KHeight);
    convolutionFilter1->setMatrix(-1, 0, 1, -2, 0, 2, -1, 0, 1);
    _filters.push_back(convolutionFilter1);
    
    Abstract3x3ConvolutionFilter * convolutionFilter2 = new Abstract3x3ConvolutionFilter(KWidth, KHeight);
    convolutionFilter2->setMatrix(4, 4, 4, 4, -32, 4, 4,  4, 4);
    _filters.push_back(convolutionFilter2);
    
    Abstract3x3ConvolutionFilter * convolutionFilter3 = new Abstract3x3ConvolutionFilter(KWidth, KHeight);
    convolutionFilter3->setMatrix(1.2,  1.2, 1.2, 1.2, -9.0, 1.2, 1.2,  1.2, 1.2);
    _filters.push_back(convolutionFilter3);
    
    
        // and here's how you might daisy-chain a bunch of filters
    
    FilterChain * foggyTexturedGlassChain = new FilterChain(KWidth, KHeight, "Weird Glass");
    foggyTexturedGlassChain->addFilter(new PerlinPixellationFilter(KWidth, KHeight, 13.f));
    foggyTexturedGlassChain->addFilter(new EmbossFilter(KWidth, KHeight, 0.5));
    foggyTexturedGlassChain->addFilter(new GaussianBlurFilter(KWidth, KHeight, 3.f));
    _filters.push_back(foggyTexturedGlassChain);


        // here's another unimaginative filter chain
    
    FilterChain * watercolorChain = new FilterChain(KWidth, KHeight, "Monet");
    watercolorChain->addFilter(new KuwaharaFilter(9));
    watercolorChain->addFilter(new LookupFilter(KWidth, KHeight, "img/lookup_miss_etikate.png"));
    watercolorChain->addFilter(new BilateralFilter(KWidth, KHeight));
    watercolorChain->addFilter(new PoissonBlendFilter("img/canvas_texture.jpg", KWidth, KHeight, 2.0));
    watercolorChain->addFilter(new VignetteFilter());
    _filters.push_back(watercolorChain);
    

    // and here's a random gradient map for posterity
    
    vector<GradientMapColorPoint> colors;
    for (float percent=0.0; percent<=1.0; percent+= 0.1)
        colors.push_back( GradientMapColorPoint(ofRandomuf(),ofRandomuf(),ofRandomuf(),percent) );
    _filters.push_back(new GradientMapFilter(colors));
    
}

void ofApp::update(){
    _video.update();
}

void ofApp::draw(){
    ofBackground(200,230,230);
    ofSetColor(255);
    ofPushMatrix();
    //ofScale(-1, 1);
    //ofTranslate(-ofGetWindowWidth(), 0);
    ofEnableBlendMode(blendMode);
    _filters[_currentFilter]->begin();
    
    //myImg.draw(0,0,myImg.getWidth()*1, myImg.getHeight()*1);
    _video.draw(0,0,KWidth,KHeight);
    _filters[_currentFilter]->end();
    ofPopMatrix();
    ofSetColor(255);
//    ofDrawBitmapString( _filters[_currentFilter]->getName() + " Filter\n(press SPACE to change filters)", ofPoint(40, 20));
}

void ofApp::keyPressed(int key){
    if (key==' ') {
        _currentFilter ++;
        if (_currentFilter>=_filters.size()) _currentFilter = 0;
    }
    if (key== '1'){
        blendMode = OF_BLENDMODE_MULTIPLY;
    }
    
    if (key== '2'){
        blendMode = OF_BLENDMODE_ADD;
    }
    if (key== '3'){
        blendMode = OF_BLENDMODE_SCREEN;
    }
    if (key== '4'){
        blendMode = OF_BLENDMODE_SUBTRACT;
    }

    else if (key=='f') ofToggleFullscreen();
}

void ofApp::exit() {
	_video.close();

}
