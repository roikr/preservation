//  ofxKinectV2.cpp
//
//  Created by Theodore Watson on 11/16/15


#include "ofMain.h"
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include "ofAppGLFWWindow.h"
#include <opencv2/opencv.hpp>
using namespace libfreenect2;

class ofxKinectV2 : public ofThread,public libfreenect2::SyncMultiFrameListener {

    public:
        ofxKinectV2():SyncMultiFrameListener(libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth) {};
        bool setup();
        void update();
        void draw();
        void exit();
        
    
    protected:
        void threadedFunction();
        bool onNewFrame(Frame::Type type, Frame *frame);

        GLFWwindow* windowP;
        libfreenect2::Registration* registration;
        string serial;
        ofTexture rgb;
        ofTexture depth;

        ofPixels depthPix;
        cv::Mat image;
        vector<ofPoint> contour;
};