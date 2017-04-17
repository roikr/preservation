#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLWindowSettings settings;
    settings.glVersionMinor=3;
    settings.glVersionMajor=3;
    settings.width = 1280;
    settings.height=800;

    ofCreateWindow(settings);
	//ofSetupOpenGL(1280,800,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
