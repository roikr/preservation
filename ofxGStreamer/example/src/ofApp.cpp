#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    gst_init(NULL,NULL);
    
    
#ifdef TARGET_LINUX
    ofDisableArbTex();
    background.setup("filesrc location="+ofToDataPath("Normal_BG.mov")+" ! qtdemux ! h264parse ! vaapidecode ! glimagesink sync=1 name=video",{"video"},true);
#elif defined TARGET_OSX
    ofEnableArbTex();
    background.setup("filesrc location="+ofToDataPath("Normal_BG.mov")+" ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=video",{"video"},false);
//    background.setup("videotestsrc ! videorate ! glimagesink sync=1 name=video",{"video"},true);
#endif
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
//    ofBackground(background.isThreadRunning() ? ofColor::green : ofColor::red);
    background.draw();
#ifndef TARGET_OSX
    foreground.mask();
#endif
    
}

void ofApp::exit() {
    background.exit();
    foreground.exit();
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
    if (!foreground.isPlaying()) {
#ifdef TARGET_LINUX
        foreground.setup("filesrc location="+ofToDataPath("bird_rgb.mov")+" ! qtdemux ! h264parse ! vaapidecode ! glimagesink sync=1 name=rgb filesrc location="+ofToDataPath("bird_a.mov")+" ! qtdemux ! h264parse ! vaapidecode ! glimagesink sync=1 name=alpha",{"rgb","alpha"},false);
#else
        foreground.setup("filesrc location="+ofToDataPath("bird_rgb.mov")+" ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=rgb filesrc location="+ofToDataPath("bird_a.mov")+" ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=alpha",{"rgb","alpha"},false);
#endif
    }
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
