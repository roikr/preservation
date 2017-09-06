#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetOrientation(OF_ORIENTATION_90_LEFT);
    //string pipeline="filesrc location="+ofxiOSGetDocumentsDirectory()+"Dark_BG.mov ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=video";
//    gstreamer.setup(pipeline,{"video"});
    vector<string> sinks={"background","foreground","alpha"};
    string pipeline="filesrc location="+ofxiOSGetDocumentsDirectory()+"Dark_BG.mov ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=background filesrc location="+ofxiOSGetDocumentsDirectory()+"Dark_FG_rgb.mov ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=foreground filesrc location="+ofxiOSGetDocumentsDirectory()+"Dark_FG_a.mov ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=alpha";
    gstreamer[0].setup(pipeline,sinks,true);
    
    string pipeline1="filesrc location="+ofxiOSGetDocumentsDirectory()+"Light_BG.mov ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=background filesrc location="+ofxiOSGetDocumentsDirectory()+"Light_FG_rgb.mov ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=foreground filesrc location="+ofxiOSGetDocumentsDirectory()+"Light_FG_a.mov ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=alpha";
    
    gstreamer[1].setup(pipeline1,sinks,true);
    selected = 0;
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(ofColor::black);
    ofPushMatrix();
    if (gstreamer[0].isAllocated()) {
        gstreamer[0].getTextures()[0].draw(0,0);
        gstreamer[0].getTextures()[1].setAlphaMask(gstreamer[0].getTextures()[2]);
        gstreamer[0].getTextures()[1].draw(0,0);
    }
    
    ofTranslate(0, 720);
    if (gstreamer[1].isAllocated()) {
        gstreamer[1].getTextures()[0].draw(0,0);
        gstreamer[1].getTextures()[1].setAlphaMask(gstreamer[1].getTextures()[2]);
        gstreamer[1].getTextures()[1].draw(0,0);
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
//    gstreamer.play();
}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){
//    gstreamer.stop();
}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){
    gstreamer[selected%2].stop();
    selected++;
    gstreamer[selected%2].play();
    
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
