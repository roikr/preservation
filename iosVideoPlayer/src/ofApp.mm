#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetOrientation(OF_ORIENTATION_90_LEFT);
    //string pipeline="filesrc location="+ofxiOSGetDocumentsDirectory()+"Dark_BG.mov ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=video";
//    gstreamer.setup(pipeline,{"video"});
    string pipeline="filesrc location="+ofxiOSGetDocumentsDirectory()+"Dark_BG.mov ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=background filesrc location="+ofxiOSGetDocumentsDirectory()+"Dark_FG_rgb.mov ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=foreground filesrc location="+ofxiOSGetDocumentsDirectory()+"Dark_FG_a.mov ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=alpha";
    gstreamer.setup(pipeline,{"background","foreground","alpha"});
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(ofColor::black);
    ofPushMatrix();
    if (gstreamer.getTextures().size()==3 && gstreamer.getTextures()[0].isAllocated() && gstreamer.getTextures()[1].isAllocated() && gstreamer.getTextures()[2].isAllocated()) {
        
        gstreamer.getTextures()[0].draw(0,0);
        gstreamer.getTextures()[1].setAlphaMask(gstreamer.getTextures()[2]);
        gstreamer.getTextures()[1].draw(0,0);
    }
    
    //ofDrawRectangle(50, 50, ofGetWidth()-100, ofGetHeight()-100);
//    gstreamer.draw();
    ofPopMatrix();
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
