//
//  ofxOSXUtils.h
//  example
//
//  Created by Roee Kremer on 14/09/2017.
//

#pragma once

#include <gst/gst.h>
#include <gst/gl/gstglcontext.h>
#include "GLFW/glfw3.h"

GstGLContext *osx_gst_gl_context_new_wrapped(GstGLDisplay *display,GLFWwindow *window);
