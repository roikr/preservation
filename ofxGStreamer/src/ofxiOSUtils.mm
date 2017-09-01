//
//  ofxiOSUtils.cpp
//  iosVideoPlayer
//
//  Created by Roee Kremer on 01/09/2017.
//

#include "ofxiOSUtils.h"
#include "ofxiOS.h"

GstGLContext * gst_gl_context_new_wrapped (GstGLDisplay *display) {
    return gst_gl_context_new_wrapped(display, (guintptr)[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2   sharegroup: [[ofxiOSGetGLView() context] sharegroup]], GST_GL_PLATFORM_EAGL, GST_GL_API_GLES2);
}
