#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    gst_init(NULL,NULL);
    ofDisableArbTex();
    
    /*
    vector<string> videos={"Dark_BG.mov","Normal_BG.mov","Dark_BG.mov"};
    for (int i=0;i<4;i++) {
        gstreamer[i].setup("filesrc location="+ofToDataPath(videos[i])+" ! qtdemux ! h264parse ! vaapidecode ! glimagesink sync=1 name=video",{"video"},true);
    }
    */
    gstreamer.setup("filesrc location="+ofToDataPath("Dark_BG.mov")+" ! qtdemux ! h264parse ! vaapidecode ! glimagesink sync=1 name=video0 filesrc location="+ofToDataPath("Normal_BG.mov")+" ! qtdemux ! h264parse ! vaapidecode ! glimagesink sync=1 name=video1 filesrc location="+ofToDataPath("Dark_BG.mov")+" ! qtdemux ! h264parse ! vaapidecode ! glimagesink sync=1 name=video2",{"video0","video1","video2"},true);
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofScale(0.25,0.25,1);

    /*
    for (int i=0;i<3;i++) {
        if (gstreamer[i].isAllocated()) {
            gstreamer[i].getTextures()[0].draw(0,0);
            ofTranslate(gstreamer[i].getTextures()[0].getWidth(),0);
        }
    }*/
    if (gstreamer.isAllocated()) {
        for (int i=0;i<3;i++) {
            gstreamer.getTextures()[i].draw(0,0);
            ofTranslate(gstreamer.getTextures()[i].getWidth(),0);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    /*
    for (int i=0;i<4;i++) {
        gstreamer[i].play();
    }
    */
    gstreamer.play();
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
