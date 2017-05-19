#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //player.load("fingers.mov");
    //cout << "fingers: " << player.getWidth() << 'x' << player.getHeight() << endl;
    //player.play();
    
     img.load("depth_239.png");
    ofPixels &pix(img.getPixels());
    
    image.create(pix.getHeight(),pix.getWidth(),CV_8UC1);
   
    updatePixels(img.getPixels());
}

void ofApp::updatePixels(ofPixels &pix) {
    for (int j=0;j<pix.getHeight();j++) {
        for (int i=0;i<pix.getWidth();i++) {
            float brightness = pix.getColor(i, j).getBrightness();
            image.at<uchar>(j,i) = brightness>128 ? 255 : 0;
        }
    }
    
    vector<vector<cv::Point> > contours;
    cv::findContours(image, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE); // CV_CHAIN_APPROX_NONE
    vector<pair<int,float> > areas;
    int i=0;
    for (auto &c:contours) {
        areas.push_back(make_pair(i++, cv::contourArea(c)));
    }
    
    cout << areas.size() << endl;
    
    sort(areas.begin(),areas.end(),[](pair<int,float> &a,pair<int,float> &b) {
        return a.second>b.second;
    });
    
    contour.clear();
    if (!areas.empty()) {
        //cout << areas.front().second << endl;
        for (auto &p:contours[areas.front().first]) {
            contour.push_back(ofPoint(p.x,p.y));
        }
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //player.update();
    //if (player.isFrameNew()) {
    
    //}
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofNoFill();
    ofSetColor(ofColor::white);
    img.draw(0, 0);
    ofSetColor(ofColor::purple);
    ofBeginShape();
    for (auto &p:contour) {
        ofVertex(p);
    }
    ofEndShape(true);
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
