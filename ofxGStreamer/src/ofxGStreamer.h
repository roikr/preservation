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
    void setup(string str,vector<string> sinks,bool bLoop);
    void threadedFunction();
    bool handleMessage(GstMessage *msg);
    
    void draw();
    void mask();
    void exit();
    
    void play();
    void stop();
    void seek();
    
    bool isAllocated();
    
    vector<ofTexture> &getTextures() { return textures;}
    
    string str;
    GMainLoop *main_loop;
    GstElement *pipeline;
    GstGLDisplay *gl_display;
    vector<ofTexture> textures;
    vector<string> sinks;
    bool bLoop;

};
