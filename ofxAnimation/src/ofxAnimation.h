//
//  ofxAnimation.h
//  backgroundPlayer
//
//  Created by Roee Kremer on 05/03/2017.
//
//

#pragma once
#include "ofMain.h"

struct animation {
    int index;
    int layer;
    ofVideoPlayer rgb;
    ofVideoPlayer alpha;
    ofTexture tex;
};

struct render {
    ofTexture tex;
};

class ofxAnimation{
    
public:
    void setup();
    void update();
    void draw();
    
    void setBackground(int index);
    int add(string name);
    void begin();
    void end();
    
    ofVideoPlayer background;
    ofVideoPlayer foreground;
    ofVideoPlayer alpha;
    
    vector<animation> players;
    
    ofFbo bgFbo,fgFbo,fbo;
    ofTexture mask;
};

