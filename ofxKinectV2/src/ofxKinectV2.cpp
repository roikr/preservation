//  ofxKinectV2.cpp
//
//  Modified by Theodore Watson on 11/16/15
//  from the Protonect example in https://github.com/OpenKinect/libfreenect2


/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2011 individual OpenKinect contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */





#include <iostream>
#include <signal.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>
#include "ofxKinectV2.h"

#include "ofAppRunner.h"

//#include <transfer_pool.h>
//#include <event_loop.h>
//#include "ofRGBPacketProcessor.h"



bool ofxKinectV2::setup() {
  if( ofGetLogLevel() == OF_LOG_VERBOSE ){
        libfreenect2::setGlobalLogger(libfreenect2::createConsoleLogger(libfreenect2::Logger::Debug));
    }else{
        libfreenect2::setGlobalLogger(libfreenect2::createConsoleLogger(libfreenect2::Logger::Warning));
    }

    libfreenect2::Freenect2 freenect2;

    if (freenect2.enumerateDevices()==0) {
      ofLogError("ofxKinectV2::setup") << "no devices connected!";
        return false;
    }

    params.add(minDistance.set("minDistance", 500, 0, 2000));
    params.add(maxDistance.set("maxDistance", 1000, 0, 4000));

    serial = freenect2.getDefaultDeviceSerialNumber();
    
      startThread(true);

    mask.allocate(1920,1080,GL_RGBA);
    
      return true;
    
}


void ofxKinectV2::update() {
    lock() ;
    if (!contour.empty()) {
        mask.begin();
        ofClear(0,0,0,0);
        
        ofSetColor(ofColor::white);
        ofFill();
        ofBeginShape();
        for (auto &p:contour) {
            ofVertex(p);
        }
        ofEndShape(true);

        mask.end();
    }
    unlock();
}

bool ofxKinectV2::onNewFrame(Frame::Type type, Frame *frame) {
    glfwMakeContextCurrent(windowP);

    if (type==Frame::Depth) {

        lock();
        float minDistance=this->minDistance;
        float maxDistance=this->maxDistance;
        unlock();

        float maxDepth = 0;
        for (int j=0;j<frame->height;j++) {
            for (int i=0;i<frame->width;i++) {
                unsigned char depth = ofMap(((float *)frame->data)[frame->width*j+i],minDistance,maxDistance,255,0,true);
                depth = (depth==255) ? 0 : depth;
                //maxDepth = max(depth,maxDepth);
                image.at<uchar>(j,i) = depth>0 ? 255 : 0;
                depthPix.getData()[frame->width*j+i]=depth;//image.at<uchar>(j,i);
            }
        }

        //ofSaveImage(depthPix,"depth_"+ofToString(ofGetFrameNum())+".png");
          
        //out << maxDepth << endl;
        vector<vector<cv::Point> > contours;
        cv::findContours(image, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE); // CV_CHAIN_APPROX_NONE
        vector<pair<int,float> > areas;
        int i=0;
        for (auto &c:contours) {
            areas.push_back(make_pair(i++, cv::contourArea(c)));
        }
          
        sort(areas.begin(),areas.end(),[](pair<int,float> &a,pair<int,float> &b) {
          return a.second>b.second;
        });

        vector<ofPoint> contour;
        if (!areas.empty()) {
            //cout << areas.front().second << endl;
              //cout << areas.front().second << endl;
            for (auto &p:contours[areas.front().first]) {
                ofPoint c;//(p.x,p.y);
                registration->apply(p.x,p.y,((float *)frame->data)[frame->width*p.y+p.x],c.x,c.y);
                // cout << p.x << '\t' << p.y << '\t' << cx << '\t' << cy << endl;
                contour.push_back(c);
            }
        } 

        lock();
        this->contour=contour;
        unlock();

    }


    GLuint textureID;
    GLint width = frame->width;
    GLint height = frame->height;
    GLenum textureTarget = GL_TEXTURE_RECTANGLE;
    GLenum glInternalFormat;
    if (type==Frame::Color || type==Frame::Depth) {
        GLenum glFormat;

        switch (type) {
            case Frame::Color:
                glInternalFormat= GL_RGBA;
                glFormat = GL_BGRA;
                break;
            case Frame::Depth:
                glInternalFormat= GL_R8 ;
                glFormat = GL_RED;
                break;

        }
        
        GLenum glType = GL_UNSIGNED_BYTE;
        
        glGenTextures(1, &textureID);
        glBindTexture(textureTarget,textureID);
        //glTexImage2D(textureTarget, 0, ofGetGlInternalFormat(pixels), (GLint)width, (GLint)height, 0, glFormat, glType, 0);  // init to black...
        glTexImage2D(textureTarget, 0, glInternalFormat, width, height, 0, glFormat, glType, 0);  // init to black...
        
        glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameterf(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        switch (type) {
            case Frame::Color:
                glTexSubImage2D(textureTarget, 0, 0, 0, width, height, glFormat, glType, frame->data);
                break;
            case Frame::Depth:
                glTexSubImage2D(textureTarget, 0, 0, 0, width, height, glFormat, glType, depthPix.getData());
                break;
        }

        glBindTexture(textureTarget,0);
        glFlush();


        ofTexture &tex(type==Frame::Color ? rgb : depth);

    
        lock();
          
        if (tex.isAllocated()) {
            glDeleteTextures(1, &tex.texData.textureID);
        }

        tex.setUseExternalTextureID(textureID);
        tex.texData.width = width;
        tex.texData.height = height;
        tex.texData.tex_w = width;
        tex.texData.tex_h = height;
        tex.texData.tex_u = 1;
        tex.texData.tex_t = 1;
        tex.texData.textureTarget = textureTarget;
        tex.texData.glInternalFormat = glInternalFormat;

        if (type==Frame::Color) {
            tex.setAlphaMask(mask.getTexture());
        }

        unlock();
    }

    glfwMakeContextCurrent(NULL); 
    return false;
}
//--------------------------------------------------------------------------------
void ofxKinectV2::threadedFunction(){

  libfreenect2::Freenect2 freenect2;

        libfreenect2::Freenect2Device *dev = 0;
        libfreenect2::PacketPipeline *pipeline = 0;

        libfreenect2::FrameMap frames;

        
        //libfreenect2::SyncMultiFrameListener * listener;
        //libfreenect2::Frame  * undistorted = NULL;
        //libfreenect2::Frame  * registered = NULL;
        //libfreenect2::Frame  * bigFrame = NULL;

    //      pipeline = new libfreenect2::CpuPacketPipeline();
      // pipeline = new libfreenect2::OpenGLPacketPipeline();
     pipeline = new libfreenect2::OpenCLPacketPipeline();

    if(pipeline==0)
    {
      ofLogError("ofxKinectV2::openKinect")  << "no pipeline ";
      return ;
    }

    dev = freenect2.openDevice(serial, pipeline);

    if(dev == 0) {
        ofLogError("ofxKinectV2::openKinect")  << "failure opening device with serial " << serial;
        return ;
    }
    
    //listener = new libfreenect2::SyncMultiFrameListener(libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth);
      //undistorted = new libfreenect2::Frame(512, 424, 4);
      //registered  = new libfreenect2::Frame(512, 424, 4);
    depthPix.allocate(512,424,1);
    image.create(424,512,CV_8UC1);
        //bigFrame = new libfreenect2::Frame(1920,1080,4);

      dev->setColorFrameListener(this);
      dev->setIrAndDepthFrameListener(this);
      dev->start();

      ofLogVerbose("ofxKinectV2::openKinect") << "device serial: " << dev->getSerialNumber();
      ofLogVerbose("ofxKinectV2::openKinect") << "device firmware: " << dev->getFirmwareVersion();

      registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
      
    ofGLFWWindowSettings settings;
  
  
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, settings.glVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, settings.glVersionMinor);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    windowP = glfwCreateWindow(1920, 1080, "", nullptr, (GLFWwindow*)ofGetWindowPtr()->getWindowContext());
    

    while(isThreadRunning()){
       
        
        sleep(10);
    }

    
    
    

  
      // TODO: restarting ir stream doesn't work!
      // TODO: bad things will happen, if frame listeners are freed before dev->stop() :(
      dev->stop();
      dev->close();
      
      
      
      // delete undistorted;
      // undistorted = NULL;

      // delete registered;
      // registered = NULL;
      
      // if (bigFrame) {
      //     delete bigFrame;
      //     bigFrame = NULL;
      // }
      
      delete registration;



    lock();
    glfwMakeContextCurrent(windowP);
    if (rgb.isAllocated()) {
        glDeleteTextures(1, &rgb.texData.textureID);
    }
    if (depth.isAllocated()) {
        glDeleteTextures(1, &depth.texData.textureID);
    }
    glfwMakeContextCurrent(NULL);
    unlock();

    glfwDestroyWindow(windowP);
}

void ofxKinectV2::draw(){
  ofNoFill();
  //ofScale(0.25, 0.25);
  //camera.draw(0, 0);

  lock() ;
  // mask.draw(0,0);
  ofSetColor(ofColor::white);
    if (rgb.isAllocated()) {
        rgb.draw(0,0);
    }

  //   if (depth.isAllocated()) {
  //       depth.draw(0,0);
  //   }

  //   ofSetColor(ofColor::purple);
  //   ofBeginShape();
  //   for (auto &p:contour) {
  //       ofVertex(p);
  //   }
  //   ofEndShape(true);

    unlock();

    ofSetColor(ofColor::white);
    
}

void ofxKinectV2::exit() {
  stopThread();
}