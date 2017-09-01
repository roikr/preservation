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
    void setup(string str);
    void threadedFunction();
    bool handleMessage(GstMessage *msg);
    void render(int texture);
    
    void draw();
    
    string str;
    GstElement *pipeline;
    GstGLDisplay *gl_display;
    ofTexture tex;
};
