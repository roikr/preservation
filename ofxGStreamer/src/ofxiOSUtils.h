//
//  ofxiOSUtils.h
//  iosVideoPlayer
//
//  Created by Roee Kremer on 01/09/2017.
//

#pragma once

#include <gst/gst.h>
#include <gst/gl/gstglcontext.h>

GstGLContext * gst_gl_context_new_wrapped (GstGLDisplay *display);
