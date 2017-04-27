#include "ofApp.h"

#include "GLFW/glfw3.h"

//#define GLFW_EXPOSE_NATIVE_COCOA
//#define GLFW_EXPOSE_NATIVE_NSGL
//#include "GLFW/glfw3native.h"

// https://blog.gvnott.com/some-usefull-facts-about-multipul-opengl-contexts/
// https://www.opengl.org/discussion_boards/showthread.php/184813-Creating-multiple-OpenGL-contexts-with-GLFW-on-different-threads
// https://github.com/glfw/glfw/blob/master/tests/sharing.c

//--------------------------------------------------------------
void ofApp::setup(){
//    ofDisableArbTex();
    startThread();
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //ofScale(0.25, 0.25);
    //camera.draw(0, 0);
    if (lock()) {
        if (tex.isAllocated()) {
            tex.draw(0,0);
        }
    unlock();
    }
    
}

void ofApp::threadedFunction()
{
    ofGLFWWindowSettings settings;
    //settings.shareContextWith = shared_ptr<ofAppBaseWindow>(ofGetWindowPtr());
    
    //ofAppGLFWWindow window;
    //glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
//    settings.width = 100;
//    settings.height = 100;
//    settings.visible=false;
//    window.setup(settings);

    int width = 128;
    int height = 128;
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, settings.glVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, settings.glVersionMinor);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    GLFWwindow* windowP = glfwCreateWindow(width, height, "", nullptr, (GLFWwindow*)ofGetWindowPtr()->getWindowContext());
    glfwMakeContextCurrent(windowP);
    
//    ofGLRenderer renderer(NULL);
//    renderer.setup();
    
    //ofFbo fbo;
    //fbo.allocate(width, height);
    //glBindFramebuffer(GL_FRAMEBUFFER, fbo.getId());
    //tex.texData=fbo.getTexture().getTextureData();
    
    ofPixels pixels;
    pixels.allocate(width, height, OF_IMAGE_COLOR);
    
    
        
//
    int i;
    i=0;
//    cout << fbo.getTexture().getTextureData().textureID << endl;
    while(isThreadRunning())
    {
        ofColor color;
        color.setHsb(0, 128, (i++)%256);
        pixels.setColor(color);
        
        GLenum glFormat = ofGetGlFormat(pixels);
        GLenum glType = ofGetGlType(pixels);
        GLenum textureTarget = GL_TEXTURE_RECTANGLE_ARB;
        
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(textureTarget,textureID);
        glTexImage2D(textureTarget, 0, ofGetGlInternalFormat(pixels), (GLint)width, (GLint)height, 0, glFormat, glType, 0);  // init to black...
        glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexSubImage2D(textureTarget, 0, 0, 0, width, height, glFormat, glType, pixels.getData());
        glBindTexture(textureTarget,0);
        glFlush();
        
        if (lock()) {
            if (this->tex.isAllocated()) {
                glDeleteTextures(1, &this->tex.texData.textureID);
            }
            this->tex.setUseExternalTextureID(textureID);
            this->tex.texData.width = width;
            this->tex.texData.height = height;
            this->tex.texData.tex_w = width;
            this->tex.texData.tex_h = height;
            this->tex.texData.tex_u = 1;
            this->tex.texData.tex_t = 1;
            this->tex.texData.textureTarget = textureTarget;
            this->tex.texData.glInternalFormat = ofGetGlInternalFormat(pixels);
    
            unlock();
        } else {
            glDeleteTextures(1, &textureID);
        }

//
        

//        renderer.viewport();
//        renderer.clear(200);
//        cout << glfwGetCurrentContext() << endl;
        sleep(25);
    }
    if (lock()) {
        if (this->tex.isAllocated()) {
            glDeleteTextures(1, &this->tex.texData.textureID);
        }
        unlock();
    }
    glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(windowP);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    stopThread();
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
