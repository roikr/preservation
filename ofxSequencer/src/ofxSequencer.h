//
//  ofxSequencer.h
//  example
//
//  Created by Roee Kremer on 06/09/2017.
//

#pragma once

#include "ofMain.h"

class ofxSequencer {
public:
    bool setup(string path,string prefix,int numDigits,int first,float framerate,float duration);
    void update();
    void draw();
    void exit();
    
    void add(ofVec2f pos);
    int numInstances() {return instances.size();}
    
    int getWidth() {return textures.empty() ? 0 : textures.front().getWidth();}
    int getHeight() {return textures.empty() ? 0 : textures.front().getHeight();}

private:    
    vector<ofTexture> textures;
    deque<pair<float,ofVec2f> > instances;
    float framerate;
    float duration;
    
};
