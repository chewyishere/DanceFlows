//
//  VoronoiFilter.h
//  filterSandbox
//
//  Created by Matthew Fargo on 2014/06/23.
//
//

#ifndef __filterSandbox__VoronoiFilter__
#define __filterSandbox__VoronoiFilter__

#include "AbstractFilter.h"

static const int NUM_POINTS = 128;

class VoronoiFilter : public AbstractFilter {
public:
	VoronoiFilter(ofTexture & texture);
	virtual ~VoronoiFilter();


    virtual void    begin();
    
protected:
    virtual string  _getFragSrc();
    
    ofTexture   _texture;
    ofPixels    _pixels;

    float       _vertices[NUM_POINTS*2];
    float       _colors[NUM_POINTS*3];
};

#endif /* defined(__filterSandbox__VoronoiFilter__) */
