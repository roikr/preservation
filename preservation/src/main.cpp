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
	ofRunApp(new ofApp());

}
