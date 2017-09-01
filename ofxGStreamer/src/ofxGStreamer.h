//
//  ofxGStreamer.h
//  DJIStreamer
//
//  Created by Roee Kremer on 15/08/2017.
//

#pragma once

#include "ofMain.h"
#include <gst/gst.h>
#include <gst/gl/gstgldisplay.h>

class ofxGStreamer : public ofThread {
    
public:
    void setup(string str,vector<string> sinks);
    void threadedFunction();
    bool handleMessage(GstMessage *msg);
    void render(int texture);
    
    void draw();
    
    vector<ofTexture> &getTextures() { return textures;}
    
    string str;
    GstElement *pipeline;
    GstGLDisplay *gl_display;
    vector<ofTexture> textures;
    vector<string> sinks;
};
