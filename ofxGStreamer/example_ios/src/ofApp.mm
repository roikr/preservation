#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetOrientation(OF_ORIENTATION_90_LEFT);
    background.setup("filesrc location="+ofxiOSGetDocumentsDirectory()+"Normal_BG.mov ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=video",{"video"},true);
    foreground.setup("filesrc location="+ofxiOSGetDocumentsDirectory()+"bird_rgb.mov ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=rgb filesrc location="+ofxiOSGetDocumentsDirectory()+"bird_a.mov ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=alpha ",{"rgb","alpha"},true);
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    background.draw();
    foreground.mask();
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::lostFocus(){

}

//--------------------------------------------------------------
void ofApp::gotFocus(){

}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){

}
