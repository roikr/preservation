//
//  ofxGStreamer.cpp
//  DJIStreamer
//
//  Created by Roee Kremer on 15/08/2017.
//

#include "ofxGStreamer.h"

#include <gst/gl/gstglcontext.h>


#include "ofxiOSUtils.h"

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
                
                /*
                Display *x11_display = glfwGetX11Display();
                cout << "x11_display: " << x11_display << endl;
                
                app->gl_display = (GstGLDisplay *)gst_gl_display_x11_new_with_display(x11_display);
                 */
                //self->gl_display =(GstGLDisplay *)gst_gl_context_get_display()
                self->gl_display = gst_gl_display_new ();
                
                gst_context_set_gl_display (display_context, self->gl_display);
                gst_element_set_context (GST_ELEMENT (msg->src), display_context);
                return TRUE;
                 
                
            } else if (g_strcmp0 (context_type, "gst.gl.app_context") == 0) {
                cout << "opengl - major: " << ofGetGLRenderer()->getGLVersionMajor() << ", minor: " << ofGetGLRenderer()->getGLVersionMinor() << endl;
                
                /*
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ofGetGLRenderer()->getGLVersionMajor());
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ofGetGLRenderer()->getGLVersionMinor());
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
                glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
                GLFWwindow* windowP = glfwCreateWindow(10, 10, "", nullptr, (GLFWwindow*)ofGetWindowPtr()->getWindowContext());
                //GLFWwindow* windowP = ofGetWindowPtr()->getWindowContext();
                
                GstGLContext *gl_context=gst_gl_context_new_wrapped(app->gl_display, (guintptr)glfwGetGLXContext(windowP), GST_GL_PLATFORM_GLX, GST_GL_API_OPENGL3);
                cout << "gl_context: " << gl_context << endl;
                */
                
                
                 
                GstContext *app_context = gst_context_new ("gst.gl.app_context", TRUE);
                GstStructure *s = gst_context_writable_structure (app_context);
                gst_structure_set (s, "context", GST_GL_TYPE_CONTEXT, gst_gl_context_new_wrapped(self->gl_display),NULL);
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
    GstFormat format = GST_FORMAT_TIME;
    GstSeekFlags flags = (GstSeekFlags) (GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT);
    if(!gst_element_seek(GST_ELEMENT(self->pipeline),1,format,flags,GST_SEEK_TYPE_SET,0,GST_SEEK_TYPE_SET,-1)) {
        cout << "unable to seek" << endl;
    }
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
    
    //cout << "texture: " << v_frame.data[0] << '\t' << v_info.width << 'x' << v_info.height << endl;
    ofTextureData data;
    data.tex_w=data.width=v_info.width;
    data.tex_h=data.height=v_info.height;
    data.tex_u=data.tex_t=1;
    data.glInternalFormat=GL_RGB8;
    tex->texData=data;
    tex->setUseExternalTextureID(*(guint *) v_frame.data[0]);
    
    //self->render(*(guint *) v_frame.data[0]);
    gst_video_frame_unmap (&v_frame);
    
    
    return TRUE;
}

void ofxGStreamer::setup(string str,vector<string> sinks) {
    char *version_utf8=gst_version_string();
    cout << version_utf8 << endl;
    g_free(version_utf8);
    this->str = str;
    this->sinks = sinks;
    startThread();
}

void ofxGStreamer::threadedFunction() {
    GMainLoop *main_loop= g_main_loop_new(NULL,FALSE);
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
    
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    g_main_loop_run(main_loop);
    
    gst_element_set_state(pipeline,GST_STATE_NULL);
    gst_object_unref(pipeline);
    
    cout << "pipeline done" << endl;
}

void ofxGStreamer::render(int texture) {
    //cout << texture << endl;
    /*
    if (!tex.isAllocated()) {
        glBindTexture(GL_TEXTURE_2D,texture);
        GLint width,height;
        //glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&width);
        //glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&height);
        glBindTexture(GL_TEXTURE_2D,0);
        width=500;
        height=500;
        tex.allocate(width,height,GL_RGBA);
        //cout << "texture: " << tex.texData.textureID << ", width: " << width << ", height: " << height << endl;
    }
    */
    //glCopyImageSubData(texture,GL_TEXTURE_2D,0,0,0,0,tex.texData.textureID,GL_TEXTURE_2D,0,0,0,0,tex.getWidth(),tex.getHeight(),1);
    
}

void ofxGStreamer::draw(){
    
    for (auto tex:textures) {
        if (tex.isAllocated()) {
            tex.draw(0, 0);
            ofTranslate(tex.getWidth(), 0);
        }
        
    }
    
    /*
    if (tex.isAllocated()) {
        // aMutex.lock();
//        ofScale(0.5,0.5);
        tex.draw(0,0);
        // aMutex.unlock();
    }
     */
}
