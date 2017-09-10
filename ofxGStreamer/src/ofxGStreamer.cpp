//
//  ofxGStreamer.cpp
//  DJIStreamer
//
//  Created by Roee Kremer on 15/08/2017.
//

#include "ofxGStreamer.h"

#include <gst/gl/gstglcontext.h>


#ifdef TARGET_LINUX
#include "GLFW/glfw3.h"
#include <gst/gl/x11/gstgldisplay_x11.h>
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include "GLFW/glfw3native.h"

// https://blog.gvnott.com/some-usefull-facts-about-multipul-opengl-contexts/
// https://www.opengl.org/discussion_boards/showthread.php/184813-Creating-multiple-OpenGL-contexts-with-GLFW-on-different-threads
// https://github.com/glfw/glfw/blob/master/tests/sharing.c

#elif defined(TARGET_OSX)

    #include "GLFW/glfw3.h"
    #include <gst/gl/cocoa/gstgldisplay_cocoa.h>
    #define GLFW_EXPOSE_NATIVE_COCOA
    #define GLFW_EXPOSE_NATIVE_NSGL
    #include "GLFW/glfw3native.h"
#else
    #include "ios/ofxiOSUtils.h"
#endif

#include <mutex>
    
std::mutex render_mutex;

static void error_cb (GstBus *bus, GstMessage *msg, ofxGStreamer *self)
{
    GError *err;
    gchar *debug_info;
    
    
    gst_message_parse_error (msg, &err, &debug_info);
    cout << "Error received from element " << GST_OBJECT_NAME (msg->src) << ": " <<  err->message << endl;
    g_clear_error (&err);
    g_free (debug_info);
    
    gst_element_set_state (self->pipeline, GST_STATE_NULL);
}


static gboolean sync_bus_call (GstBus * bus, GstMessage * msg, ofxGStreamer *self)
{
    // return FALSE;
    switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_NEED_CONTEXT:
        {
            const gchar *context_type;
            
            gst_message_parse_context_type (msg, &context_type);
            g_print ("got need context %s\n", context_type);
            
            if (g_strcmp0 (context_type, GST_GL_DISPLAY_CONTEXT_TYPE) == 0) {
                cout << "display request" << endl;
                
                GstContext *display_context =
                gst_context_new (GST_GL_DISPLAY_CONTEXT_TYPE, TRUE);
#ifdef TARGET_LINUX
                
                Display *x11_display = glfwGetX11Display();
                cout << "x11_display: " << x11_display << endl;
                self->gl_display = (GstGLDisplay *)gst_gl_display_x11_new_with_display(x11_display);
#elif defined TARGET_OSX
               return FALSE;
//                self->gl_display = (GstGLDisplay *)gst_gl_display_cocoa_new();
                
                
#else
                self->gl_display = gst_gl_display_new ();
#endif
                
                gst_context_set_gl_display (display_context, self->gl_display);
                gst_element_set_context (GST_ELEMENT (msg->src), display_context);
                return TRUE;
                 
                
            } else if (g_strcmp0 (context_type, "gst.gl.app_context") == 0) {
                
                GstContext *app_context = gst_context_new ("gst.gl.app_context", TRUE);
                GstStructure *s = gst_context_writable_structure (app_context);
                
                cout << "opengl - major: " << ofGetGLRenderer()->getGLVersionMajor() << ", minor: " << ofGetGLRenderer()->getGLVersionMinor() << endl;

#if defined TARGET_LINUX
                
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ofGetGLRenderer()->getGLVersionMajor());
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ofGetGLRenderer()->getGLVersionMinor());
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
                glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
                GLFWwindow* windowP = glfwCreateWindow(10, 10, "", nullptr, (GLFWwindow*)ofGetWindowPtr()->getWindowContext());
                //GLFWwindow* windowP = ofGetWindowPtr()->getWindowContext();
                GstGLContext *gl_context=gst_gl_context_new_wrapped(self->gl_display, (guintptr)glfwGetGLXContext(windowP), GST_GL_PLATFORM_GLX, GST_GL_API_OPENGL3);
                gst_structure_set (s, "context", GST_GL_TYPE_CONTEXT, gl_context,NULL);
                cout << "gl_context: " << gl_context << endl;
#elif defined TARGET_OSX
                return FALSE;
                
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ofGetGLRenderer()->getGLVersionMajor());
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ofGetGLRenderer()->getGLVersionMinor());
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
                glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
                GLFWwindow* windowP = glfwCreateWindow(10, 10, "", nullptr, (GLFWwindow*)ofGetWindowPtr()->getWindowContext());

                GstGLContext *gl_context=gst_gl_context_new_wrapped(self->gl_display, (guintptr)glfwGetNSGLContext(windowP), GST_GL_PLATFORM_CGL, GST_GL_API_OPENGL3);
                gst_structure_set (s, "context", GST_GL_TYPE_CONTEXT, gl_context,NULL);
                cout << "gl_context: " << gl_context << endl;

                
#else
                
                
                gst_structure_set (s, "context", GST_GL_TYPE_CONTEXT, gst_gl_context_new_wrapped(self->gl_display),NULL);
#endif
                gst_element_set_context (GST_ELEMENT (msg->src), app_context);
                return TRUE;
                
                
                
                
            }
            break;
        }
        default:
            break;
    }
    return FALSE;
}

static void state_changed_cb (GstBus *bus, GstMessage *msg, ofxGStreamer *self)
{
    GstState old_state, new_state, pending_state;
    gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
    /* Only pay attention to messages coming from the pipeline, not its children */
    if (GST_MESSAGE_SRC (msg) == GST_OBJECT (self->pipeline)) {
        cout << "State changed to " << gst_element_state_get_name(new_state) << endl;
    }
}

static void eos_cb (GstBus *bus, GstMessage *msg, ofxGStreamer *self) {
    
    cout << "eos" << endl;
    if (!self->bLoop) {
        gst_element_set_state(self->pipeline, GST_STATE_PAUSED);
    }
    self->seek();
    
}


static gboolean drawCallback(GstElement * gl_sink,GstGLContext *context, GstSample *sample,ofTexture *tex) {
//    cout << "drawCallback" << endl;
    
    GstVideoFrame v_frame;
    GstVideoInfo v_info;
    //guint texture = 0;
    GstBuffer *buf = gst_sample_get_buffer (sample);
    GstCaps *caps = gst_sample_get_caps (sample);
    
    gst_video_info_from_caps (&v_info, caps);
    
    if (!gst_video_frame_map (&v_frame, &v_info, buf, GstMapFlags(GST_MAP_READ | GST_MAP_GL))) {
        g_warning ("Failed to map the video buffer");
        return TRUE;
    }
    
    render_mutex.lock();

#if  defined TARGET_LINUX
    GLuint texture = *(GLuint *) v_frame.data[0];
    //cout << "texture: " << texture << endl;
    
    if (!tex->isAllocated()) {
        glBindTexture(GL_TEXTURE_2D,texture);
        GLint width,height;
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&height);
        glBindTexture(GL_TEXTURE_2D,0);
        
        tex->allocate(width,height,GL_RGBA8);
        cout << "texture: " << tex->texData.textureID << ", width: " << width << ", height: " << height << endl;
    }
    glCopyImageSubData(texture,GL_TEXTURE_2D,0,0,0,0,tex->texData.textureID,GL_TEXTURE_2D,0,0,0,0,tex->getWidth(),tex->getHeight(),1);
    //glDeleteTextures(1, &texture);
#elif defined TARGET_OSX
    GLuint texture = *(GLuint *) v_frame.data[0];
    cout << "texture: " << texture << endl;
#else
    //cout << "texture: " << v_frame.data[0] << '\t' << v_info.width << 'x' << v_info.height << endl;
    ofTextureData data;
    data.tex_w=data.width=v_info.width;
    data.tex_h=data.height=v_info.height;
    data.tex_u=data.tex_t=1;
    data.glInternalFormat=GL_RGB8;
    tex->texData=data;
    tex->setUseExternalTextureID(*(guint *) v_frame.data[0]);
#endif

    render_mutex.unlock();
    //self->render(*(guint *) v_frame.data[0]);
    gst_video_frame_unmap (&v_frame);
    
#ifdef TARGET_OSX
    return FALSE;
#else
    return TRUE;
#endif
}

void ofxGStreamer::setup(string str,vector<string> sinks,bool bLoop) {
    char *version_utf8=gst_version_string();
    cout << version_utf8 << endl;
    g_free(version_utf8);
    this->str = str;
    this->sinks = sinks;
    this->bLoop = bLoop;
    startThread();
}

void ofxGStreamer::threadedFunction() {
    main_loop= g_main_loop_new(NULL,FALSE);
    GError *error = NULL;
    
    pipeline = gst_parse_launch(str.c_str(), &error);
    
    if (error) {
        cout << "unable to build pipeline: " << error->message << endl;
        std::exit(-1);
    }
    
    GstBus *bus = gst_element_get_bus(pipeline);
    gst_bus_add_signal_watch(bus);
    
    gst_bus_enable_sync_message_emission(bus);
    
    g_signal_connect(G_OBJECT(bus), "message::state-changed", (GCallback)state_changed_cb, this);
    g_signal_connect (G_OBJECT (bus), "message::error", (GCallback)error_cb, this);
    g_signal_connect (G_OBJECT (bus), "message::eos", (GCallback)eos_cb, this);
    g_signal_connect (G_OBJECT (bus), "sync-message", (GCallback)sync_bus_call, this);
    
    gst_object_unref(bus);
    
    
    textures.assign(sinks.size(),ofTexture());
    
    int i=0;
    for (auto s:sinks) {
        GstElement *glimagesink = gst_bin_get_by_name(GST_BIN(pipeline),s.c_str());
        g_signal_connect(G_OBJECT(glimagesink),"client-draw",G_CALLBACK(drawCallback),&textures[i++]);
        gst_object_unref(glimagesink);
    }
    
    gst_element_set_state(pipeline,GST_STATE_PLAYING);
    g_main_loop_run(main_loop);
    g_main_loop_unref (main_loop);
    main_loop = NULL;
    
    gst_element_set_state(pipeline,GST_STATE_NULL);
    gst_object_unref(pipeline);
    gst_object_unref (pipeline);
    pipeline = NULL;
    
    cout << "pipeline done" << endl;
}

void ofxGStreamer::play() {
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
}

void ofxGStreamer::stop() {
    gst_element_set_state(pipeline, GST_STATE_PAUSED);
    if (!bLoop) {
        seek();
    }
}

bool ofxGStreamer::isAllocated() {

    bool ret=true;

    render_mutex.lock();

    if (textures.empty()) {
        ret= false;
    } else {
        for (auto tex:textures) {
            if (!tex.isAllocated()) {
                ret=false;
                break;
            }
        }
    }
    render_mutex.unlock();
    return ret;
}

void ofxGStreamer::draw(){
    
    render_mutex.lock();
    ofPushMatrix();
    for (auto tex:textures) {
        if (tex.isAllocated()) {
            tex.draw(0, 0);
            ofTranslate(tex.getWidth(), 0);
        }
    }
    ofPopMatrix();
    render_mutex.unlock();
    
}

void ofxGStreamer::mask() {
    render_mutex.lock();
    if (textures.size()==2 && textures[0].isAllocated() && textures[1].isAllocated()) {
        textures[0].setAlphaMask(textures[1]);
        textures[0].draw(0,0);
    }
    render_mutex.unlock();
}

void ofxGStreamer::exit() {
    g_main_loop_quit(main_loop);
}

void ofxGStreamer::seek() {
    GstFormat format = GST_FORMAT_TIME;
    GstSeekFlags flags = (GstSeekFlags) (GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT);
    if(!gst_element_seek(GST_ELEMENT(pipeline),1,format,flags,GST_SEEK_TYPE_SET,0,GST_SEEK_TYPE_SET,-1)) {
        cout << "unable to seek" << endl;
    }
}
