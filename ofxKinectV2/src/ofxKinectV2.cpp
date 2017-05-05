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



#include "ofxKinectV2.h"

#include "ofAppRunner.h"

#include <iostream>
#include <signal.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>
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

    serial = freenect2.getDefaultDeviceSerialNumber();
    
      startThread(true);

      return true;
    
}


void ofxKinectV2::update() {

}

bool ofxKinectV2::onNewFrame(Frame::Type type, Frame *frame) {
        glfwMakeContextCurrent(windowP);

        GLuint textureID;
        GLint width = frame->width;
        GLint height = frame->height;
        GLenum glInternalFormat= GL_RGBA;
        GLenum glFormat = GL_BGRA;
        GLenum glType = GL_UNSIGNED_BYTE;
        GLenum textureTarget = GL_TEXTURE_RECTANGLE_ARB;
 
        glGenTextures(1, &textureID);
        glBindTexture(textureTarget,textureID);
        //glTexImage2D(textureTarget, 0, ofGetGlInternalFormat(pixels), (GLint)width, (GLint)height, 0, glFormat, glType, 0);  // init to black...
        glTexImage2D(textureTarget, 0, glInternalFormat, width, height, 0, glFormat, glType, 0);  // init to black...
        
        glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexSubImage2D(textureTarget, 0, 0, 0, width, height, glFormat, glType, frame->data);
        glBindTexture(textureTarget,0);
        glFlush();
        
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

          unlock();


        glfwMakeContextCurrent(NULL);
        
        return false;
}
//--------------------------------------------------------------------------------
void ofxKinectV2::threadedFunction(){

  libfreenect2::Freenect2 freenect2;

        libfreenect2::Freenect2Device *dev = 0;
        libfreenect2::PacketPipeline *pipeline = 0;

        libfreenect2::FrameMap frames;

        libfreenect2::Registration* registration;
        //libfreenect2::SyncMultiFrameListener * listener;
        libfreenect2::Frame  * undistorted = NULL;
        libfreenect2::Frame  * registered = NULL;
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
      undistorted = new libfreenect2::Frame(512, 424, 4);
      registered  = new libfreenect2::Frame(512, 424, 4);
        //bigFrame = new libfreenect2::Frame(1920,1080,4);

      dev->setColorFrameListener(this);
      //dev->setIrAndDepthFrameListener(listener);
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

    lock();
    glfwMakeContextCurrent(windowP);
    if (tex.isAllocated()) {
        glDeleteTextures(1, &tex.texData.textureID);
    }
    glfwMakeContextCurrent(NULL);
    unlock();
    
    
    glfwDestroyWindow(windowP);

  
      // TODO: restarting ir stream doesn't work!
      // TODO: bad things will happen, if frame listeners are freed before dev->stop() :(
      dev->stop();
      dev->close();
      
      
      
      delete undistorted;
      undistorted = NULL;

      delete registered;
      registered = NULL;
      
      // if (bigFrame) {
      //     delete bigFrame;
      //     bigFrame = NULL;
      // }
      
      delete registration;
}

void ofxKinectV2::draw(){
    
    //ofScale(0.25, 0.25);
    //camera.draw(0, 0);
    lock() ;
      if (tex.isAllocated()) {
          tex.draw(0,0);
      }
    unlock();
    
}

void ofxKinectV2::exit() {
  stopThread();
}