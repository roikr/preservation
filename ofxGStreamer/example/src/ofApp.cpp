#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    gst_init(NULL,NULL);
    ofDisableArbTex();
    
#ifdef TARGET_LINUX
    background.setup("filesrc location="+ofToDataPath("Normal_BG.mov")+" ! qtdemux ! h264parse ! vaapidecode ! glimagesink sync=1 name=video",{"video"},true);
    foreground.setup("filesrc location="+ofToDataPath("bird_rgb.mov")+" ! qtdemux ! h264parse ! vaapidecode ! glimagesink sync=1 name=rgb filesrc location="+ofToDataPath("bird_a.mov")+" ! qtdemux ! h264parse ! vaapidecode ! glimagesink sync=1 name=alpha ",{"rgb","alpha"},true);

    background.start();
    foreground.start();
#elif defined TARGET_OSX
    background.setup("filesrc location="+ofToDataPath("Normal_BG.mov")+" ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=video",{"video"},true);
    
//    background.setup("videotestsrc ! videorate ! glimagesink sync=1 name=video",{"video"},true);
    
    foreground.setup("filesrc location="+ofToDataPath("bird_rgb.mov")+" ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=rgb filesrc location="+ofToDataPath("bird_a.mov")+" ! qtdemux ! h264parse ! vtdec ! glimagesink sync=1 name=alpha ",{"rgb","alpha"},true);
    
    background.start();
#endif
    
}

//--------------------------------------------------------------
void ofApp::update(){
    background.update();
    foreground.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    background.draw();
    foreground.mask();
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
