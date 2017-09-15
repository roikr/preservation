//
//  ofxOSXUtils.m
//  example
//
//  Created by Roee Kremer on 14/09/2017.
//

#include "ofxOSXUtils.h"
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#include "GLFW/glfw3native.h"

GstGLContext * osx_gst_gl_context_new_wrapped (GstGLDisplay *display,GLFWwindow *window) {
    NSOpenGLContext *context = glfwGetNSGLContext(window);
    
    //CGLError err = 0;
    
    CGLContextObj ctx;
    CGLCreateContext([[context pixelFormat] CGLPixelFormatObj], [context CGLContextObj], &ctx);
    

    // Enable the multi-threading
    CGLError err =  CGLEnable( ctx, kCGLCEMPEngine);
    
    if (err != kCGLNoError )
    {
        NSLog(@"CGLError: Multi-threaded execution is possibly not available");
        // Multi-threaded execution is possibly not available
        // Insert your code to take appropriate action
    }

    
    
    return gst_gl_context_new_wrapped(display,(guintptr)ctx, GST_GL_PLATFORM_CGL, GST_GL_API_OPENGL3);
}

