#include "ofApp.h"
#include "ofGstVideoPlayer.h"
#include <gst/gl/gl.h>
#include <gst/gl/gstglcontext.h>

//#include <gst/gl/gstglcontext.h>


static GstBusSyncReply
sync_bus_call (GstBus *bus, GstMessage *msg, ofApp *app)
{
    //    cout << gst_message_type_get_name((GST_MESSAGE_TYPE (msg))) << endl;
    switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:
            GError *error;
            gchar *debug;
            gst_message_parse_error(msg, &error, &debug);
            cout << debug << endl;
            break;
            
        case GST_MESSAGE_NEED_CONTEXT:
        {
            const gchar *context_type;
            GstContext *context = NULL;
            
            gst_message_parse_context_type (msg, &context_type);
            g_print("got need context %s\n", context_type);
            
            if (g_strcmp0 (context_type, "gst.gl.app_context") == 0) {
                
                
                //[EAGLContext setCurrentContext: app->context];
                GstGLContext *gl_context;//=gst_gl_context_new_wrapped(gst_gl_display_new (), (guintptr)[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2   sharegroup: [[ofxiOSGetGLView() context] sharegroup]], GST_GL_PLATFORM_EAGL, GST_GL_API_GLES2);
                GstStructure *s;
                
                context = gst_context_new ("gst.gl.app_context", TRUE);
                s = gst_context_writable_structure (context);
                gst_structure_set (s, "context", GST_TYPE_GL_CONTEXT, gl_context, NULL);
                gst_element_set_context (GST_ELEMENT (msg->src), context);
            }
            if (context)
                gst_context_unref (context);
            
        } break;
            
        default:
            break;
    }
    
    return GST_BUS_PASS;
}


static GstPadProbeReturn cb_have_data (GstPad *pad,GstPadProbeInfo *info,ofApp *sink)

{
    
    GstBuffer *buf= GST_PAD_PROBE_INFO_BUFFER (info);
    
    if (!sink->y.isAllocated()) {
        GstCaps *caps = gst_pad_get_current_caps(pad);
        GstStructure *s = gst_caps_get_structure(caps, 0);
        
        int width;
        int height;
        
        bool res = gst_structure_get_int (s, "width", &width);
        res |= gst_structure_get_int (s, "height", &height);
        
        if (!res) {
            g_print ("could not get snapshot dimension\n");
            std::exit (-1);
        }
        
        sink->allocateGL(width,height,gst_buffer_n_memory(buf));
        
        int num;
        int denom;
        gst_structure_get_fraction (s,"framerate",&num,&denom);
        
        //        sink->frameDuration = (float)denom/(float)num;
        
        cout << width << 'x' << height << '\t' << gst_structure_get_string(s, "format") << endl;//'\t' << gst_buffer_n_memory(buffer) ;
        
        //for(vector<frame >::iterator iter=frames.begin();iter!=frames.end();iter++) {
    
    }
    sink->processGL(buf);
    
    return GST_PAD_PROBE_OK;
}

void ofApp::allocateGL(int width,int height,int channels) {
    switch (channels) {
        case 1:
            y.texData.width=width;
            y.texData.height=height;
            y.texData.tex_w = y.texData.width;//ofNextPow2(y.texData.width);
            y.texData.tex_h = y.texData.height;//ofNextPow2(y.texData.height);
            y.texData.tex_t = 1.0;//y.texData.width / y.texData.tex_w;
            y.texData.tex_u = 1.0;//y.texData.height / y.texData.tex_h;
            y.texData.glInternalFormat = GL_RGBA;
            break;
        case 2:
            y.texData.width=width;
            y.texData.height=height;
            y.texData.tex_w = y.texData.width;//ofNextPow2(y.texData.width);
            y.texData.tex_h = y.texData.height;//ofNextPow2(y.texData.height);
            y.texData.tex_t = 1.0;//y.texData.width / y.texData.tex_w;
            y.texData.tex_u = 1.0;//y.texData.height / y.texData.tex_h;
            y.texData.textureTarget=GL_TEXTURE_2D;
            y.texData.glInternalFormat = GL_R8;
            
            uv.texData.width=width/2;
            uv.texData.height=height/2;
            uv.texData.tex_w = uv.texData.width;
            uv.texData.tex_h = uv.texData.height;
            uv.texData.tex_t = 1.0;
            uv.texData.tex_u = 1.0;
            uv.texData.textureTarget=GL_TEXTURE_2D;
            uv.texData.glInternalFormat =  GL_RG8;
            break;
    }
}

void ofApp::processGL(GstBuffer *buf) {

    //    GstCaps *caps = gst_pad_get_current_caps(pad);
    //    GstStructure *s = gst_caps_get_structure(caps, 0);
    //
    //    int width;
    //    int height;
    //    int num;
    //    int denom;
    //
    //    gst_structure_get_int (s, "width", &width);
    //    gst_structure_get_int (s, "height", &height);
    //    gst_structure_get_fraction (s,"framerate",&num,&denom);
    //
    //    cout << width << 'x' << height << '\t' << num << '/' << denom << '\t' << gst_structure_get_string(s, "format") << '\t' << gst_buffer_n_memory(buf) ;
    
    //    sink->buffers.clear();
    
    
    switch (gst_buffer_n_memory(buf)) {
        case 1: {
            GstMemory *mem0 = gst_buffer_get_memory(buf, 0);
            y.setUseExternalTextureID(gst_gl_memory_get_texture_id((GstGLMemory *)mem0));
            //sink->buffers.push_back(buffer(gst_gl_memory_get_texture_id((GstGLMemory *)mem0),0,GST_BUFFER_PTS(buf)/1000000000.0));
            gst_memory_unref(mem0);
        }   break;
        case 2: {
            //sink->aMutex.lock();
            GstMemory *mem0 = gst_buffer_get_memory(buf, 0);
            GstMemory *mem1 = gst_buffer_get_memory(buf, 1);
            
            //            cout << '\t' << gst_is_gl_memory(mem0) << endl;
            
            //sink->buffers.push_back(buffer(gst_gl_memory_get_texture_id((GstGLMemory *)mem0),gst_gl_memory_get_texture_id((GstGLMemory *)mem1),GST_BUFFER_PTS(buf)/1000000000.0));
            
            y.setUseExternalTextureID(gst_gl_memory_get_texture_id((GstGLMemory *)mem0));
            uv.setUseExternalTextureID(gst_gl_memory_get_texture_id((GstGLMemory *)mem1));
            
            gst_memory_unref(mem0);
            gst_memory_unref(mem1);
            
            //sink->aMutex.unlock();
            
        }   break;
        default:
            break;
    }
    
    
    
    //    //sink->aMutex.lock();
    //    GstMemory *mem0 = gst_buffer_get_memory(buf, 0);
    //    GstMemory *mem1 = gst_buffer_get_memory(buf, 1);
    //
    ////    cout << '\t' << gst_is_gl_memory(mem0) << endl;
    //
    //    sink->buffers.push_back(buffer(gst_gl_memory_get_texture_id((GstGLMemory *)mem0),gst_gl_memory_get_texture_id((GstGLMemory *)mem1),GST_BUFFER_PTS(buf)/1000000000.0));
    //    gst_memory_unref(mem0);
    //    gst_memory_unref(mem1);
    
    //  //sink->aMutex.unlock();
    
}


//--------------------------------------------------------------
void ofApp::setup(){
    
    
//    player.setPlayer(ofPtr<ofGstVideoPlayer>(new ofGstVideoPlayer));
//    player.loadMovie("short.mov");
//    player.play();
    
    string str("filesrc location="+ofToDataPath("short.mov")+" ! qtdemux ! vtdec ! video/x-raw(memory:GLMemory),width=1280,height=720,framterate=30/1 ! fakesink name=video sync=true");
    
    GError *error = NULL;
    m_pipeline = (GstPipeline*)gst_parse_launch(str.c_str(), &error);
    
    if (error) {
        gchar *message = g_strdup_printf("Unable to build pipeline: %s", error->message);
        g_clear_error (&error);
        g_print("%s",message);
        //[self setUIMessage:message];
        g_free (message);
        std::exit(-1);
    }
    
    GValue boolVal = { 0, };
    g_value_init (&boolVal, G_TYPE_BOOLEAN);
    g_value_set_boolean(&boolVal, TRUE);
    // use to detect eos on one of the sources to send for the while pipeline using GST_MESSAGE_ELEMENT
    g_object_set_property(G_OBJECT (m_pipeline), "message-forward", &boolVal);
    
    string name="video";
    GstElement *sink=gst_bin_get_by_name(GST_BIN(m_pipeline),name.c_str());
    GstPad *m_pad=gst_element_get_static_pad(sink, "sink");
    gst_pad_add_probe (m_pad, GST_PAD_PROBE_TYPE_BUFFER,(GstPadProbeCallback) cb_have_data, this, NULL);
    gst_object_unref (m_pad);

    
//    rgb.setup(m_pipeline,"rgb");
//    alpha.setup(m_pipeline, "alpha");
//    cam.setup(m_pipeline, "camera");
//    
//    shader.load("Shaders/Shader.vsh", "Shaders/Shader.fsh");
//    mask.load("Shaders/Shader.vsh", "Shaders/Combine.fsh");
    
    
    m_bus = gst_element_get_bus ((GstElement*)m_pipeline);
    gst_bus_set_sync_handler(m_bus, (GstBusSyncHandler)sync_bus_call, this, NULL);
}

//--------------------------------------------------------------
void ofApp::update(){
    //player.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    //player.draw(0,0);
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
