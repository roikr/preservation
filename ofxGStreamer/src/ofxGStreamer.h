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


class ofxGStreamer {
    
public:
    void setup(string str,vector<string> sinks,bool bLoop);
    void update();
    void draw();
    void mask();
    void exit();
    
    void asyncMessage(GstMessage *msg);
    void stop();
    void seek();
    
    bool isAllocated();
    bool isPlaying() {return bPlaying;}
    
    void threadedFunction();
    
    vector<ofTexture> &getTextures() { return textures;}
    
    GstElement *pipeline;
    GstGLDisplay *gl_display;
    vector<ofTexture> textures;
    bool bLoop;
    bool bPlaying;

};
