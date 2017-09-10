#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
#ifdef TARGET_LINUX
    ofGLWindowSettings settings;
    settings.glVersionMajor=4;
    settings.glVersionMinor=3;
    settings.width = 1280;
    settings.height=800;
    ofCreateWindow(settings);
#elif defined TARGET_OSX
    ofGLWindowSettings settings;
    settings.glVersionMajor=3;
    settings.glVersionMinor=3;
    settings.width = 1280;
    settings.height=800;
    ofCreateWindow(settings);
#else
	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context
#endif
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
