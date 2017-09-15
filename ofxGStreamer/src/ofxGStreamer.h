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


class ofxGStreamer:public ofThread {
    
public:
    void setup(string str,vector<string> sinks,bool bLoop);
    void update();
    void draw();
    void mask();
    void exit();
    
    void asyncMessage(GstMessage *msg);
    void seek();
    
    bool isAllocated();
    bool isPlaying() {return isThreadRunning();}
    
    void threadedFunction();
    
    vector<ofTexture> &getTextures() { return textures;}
    
    GstElement *pipeline;
    GstGLDisplay *gl_display;
    GMainLoop *main_loop;
    
    string str;
    vector<string> sinks;
    vector<ofTexture> textures;
    
    bool bLoop;

};
