#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    vector<string> videos={"Dark_BG.mov","Normal_BG.mov","Dark_BG.mov"};
    for (int i=0;i<3;i++) {
        gstreamer[i].setup("filesrc location="+ofToDataPath(videos[i])+" ! qtdemux ! h264parse ! vaapidecode ! glimagesink sync=1 name=video",{"video"});
    }
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofScale(0.25,0.25,1);
    for (int i=0;i<3;i++) {
        if (gstreamer[i].isAllocated()) {
            gstreamer[i].getTexture[0].draw();
            ofTranslate(gstreamer[i].getTexture[0].getWidth(),0);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    for (int i=0;i<3;i++) {
        gstreamer[i].play();
    }
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
