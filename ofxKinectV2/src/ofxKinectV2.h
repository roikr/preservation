//  ofxKinectV2.cpp
//
//  Created by Theodore Watson on 11/16/15


#include "ofMain.h"
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include "ofAppGLFWWindow.h"
using namespace libfreenect2;

class ofxKinectV2 : public ofThread,public SyncMultiFrameListener {

    public:
        ofxKinectV2():SyncMultiFrameListener(libfreenect2::Frame::Color) {};// | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth) {};
        bool setup();
        void update();
        void draw();
        void exit();
        
    
    protected:
        void threadedFunction();
        bool onNewFrame(Frame::Type type, Frame *frame);

        GLFWwindow* windowP;

        string serial;
        ofTexture tex;
        
};