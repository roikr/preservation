#include "ofApp.h"
#include "ofGstVideoPlayer.h"
#include <gst/gl/gstglcontext.h>
#include <gst/gl/x11/gstgldisplay_x11.h>

#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#include "GLFW/glfw3native.h"

// https://blog.gvnott.com/some-usefull-facts-about-multipul-opengl-contexts/
// https://www.opengl.org/discussion_boards/showthread.php/184813-Creating-multiple-OpenGL-contexts-with-GLFW-on-different-threads
// https://github.com/glfw/glfw/blob/master/tests/sharing.c


static gboolean
sync_bus_call (GstBus * bus, GstMessage * msg, ofApp *app)
{
    // return FALSE;
    switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_NEED_CONTEXT:
        {
            const gchar *context_type;

            gst_message_parse_context_type (msg, &context_type);
            g_print ("got need context %s\n", context_type);

            if (g_strcmp0 (context_type, GST_GL_DISPLAY_CONTEXT_TYPE) == 0) {
                GstContext *display_context =
                gst_context_new (GST_GL_DISPLAY_CONTEXT_TYPE, TRUE);
                Display *x11_display = glfwGetX11Display();
                cout << "x11_display: " << x11_display << endl;   

                app->gl_display = (GstGLDisplay *)gst_gl_display_x11_new_with_display(x11_display);
                gst_context_set_gl_display (display_context, app->gl_display);
                gst_element_set_context (GST_ELEMENT (msg->src), display_context);
                return TRUE;
            } else if (g_strcmp0 (context_type, "gst.gl.app_context") == 0) {
                cout << "opengl - major: " << ofGetGLRenderer()->getGLVersionMajor() << ", minor: " << ofGetGLRenderer()->getGLVersionMinor() << endl;

            
                glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ofGetGLRenderer()->getGLVersionMajor());
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ofGetGLRenderer()->getGLVersionMinor());
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
                glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
                GLFWwindow* windowP = glfwCreateWindow(10, 10, "", nullptr, (GLFWwindow*)ofGetWindowPtr()->getWindowContext());
                //GLFWwindow* windowP = ofGetWindowPtr()->getWindowContext();
            
                GstGLContext *gl_context=gst_gl_context_new_wrapped(app->gl_display, (guintptr)glfwGetGLXContext(windowP), GST_GL_PLATFORM_GLX, GST_GL_API_OPENGL3);
                cout << "gl_context: " << gl_context << endl;

                GstContext *app_context = gst_context_new ("gst.gl.app_context", TRUE);
                GstStructure *s = gst_context_writable_structure (app_context);
                gst_structure_set (s, "context", GST_GL_TYPE_CONTEXT, gl_context,
                    NULL);
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


static gboolean drawCallback (GstElement * gl_sink, GstGLContext *context, GstSample * sample, ofApp *app)
{
    GstVideoFrame v_frame;
    GstVideoInfo v_info;
    guint texture = 0;
     GstBuffer *buf = gst_sample_get_buffer (sample);
    GstCaps *caps = gst_sample_get_caps (sample);

    gst_video_info_from_caps (&v_info, caps);

    if (!gst_video_frame_map (&v_frame, &v_info, buf, GST_MAP_READ | GST_MAP_GL)) {
        g_warning ("Failed to map the video buffer");
        return TRUE;
    }

    app->render(*(guint *) v_frame.data[0]);
    gst_video_frame_unmap (&v_frame);

    return TRUE;
}

void ofApp::render(int texture) {
    if (!tex.isAllocated()) {
        glBindTexture(GL_TEXTURE_2D,texture);  
        GLint width,height;
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&height);
        glBindTexture(GL_TEXTURE_2D,0); 
         
        tex.allocate(width,height,GL_RGBA8);
        cout << "texture: " << tex.texData.textureID << ", width: " << width << ", height: " << height << endl;
    } 
    
    glCopyImageSubData(texture,GL_TEXTURE_2D,0,0,0,0,tex.texData.textureID,GL_TEXTURE_2D,0,0,0,0,tex.getWidth(),tex.getHeight(),1);
}    
    


//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    gst_init(NULL,NULL);
    startThread();
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (tex.isAllocated()) {
        // aMutex.lock();
        ofScale(0.5,0.5);
        tex.draw(0,0);
        // aMutex.unlock();
    }
}

void ofApp::threadedFunction() {



    string str("multifilesrc loop=true location=/home/ubuntu/feast.mp4 ! qtdemux ! h264parse ! vaapidecode ! vaapipostproc ! glimagesink sync=1 name=video");
    // string str("videotestsrc ! video/x-raw, width=1280, height=544, framerate=(fraction)30/1,format=RGBA ! glupload !  fakesink name=video sync=1");

    GMainLoop *main_loop = g_main_loop_new (NULL, FALSE);  /* GLib's Main Loop */
    GError *error = NULL;
    
   
    GstPipeline * pipeline = (GstPipeline*)gst_parse_launch(str.c_str(), &error);
    
    if (error) {
        gchar *message = g_strdup_printf("Unable to build pipeline: %s", error->message);
        g_clear_error (&error);
        g_print("%s",message);
        //[self setUIMessage:message];
        g_free (message);
        std::exit(-1);
    }
    
    // GValue boolVal = { 0, };
    // g_value_init (&boolVal, G_TYPE_BOOLEAN);
    // g_value_set_boolean(&boolVal, TRUE);
    // // use to detect eos on one of the sources to send for the while pipeline using GST_MESSAGE_ELEMENT
    // g_object_set_property(G_OBJECT (m_pipeline), "message-forward", &boolVal);

    GstBus * bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    gst_bus_add_signal_watch (bus);
    gst_bus_enable_sync_message_emission (bus);
    g_signal_connect (bus, "sync-message", G_CALLBACK (sync_bus_call), this);
    gst_object_unref (bus);
    
    string name="video";

    GstElement *glimagesink=gst_bin_get_by_name(GST_BIN(pipeline),name.c_str());
    g_signal_connect (G_OBJECT (glimagesink), "client-draw", G_CALLBACK (drawCallback), this);
    gst_object_unref (glimagesink);
    


    gst_element_set_state((GstElement*)pipeline, GST_STATE_PLAYING);
    g_main_loop_run (main_loop);

    gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_NULL);
    gst_object_unref (pipeline);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
