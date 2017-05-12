#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    player.load("fingers.mov");
    cout << "fingers: " << player.getWidth() << 'x' << player.getHeight() << endl;
    player.play();
    image.create(player.getWidth(),player.getHeight(),CV_8UC1);
}

//--------------------------------------------------------------
void ofApp::update(){
    player.update();
    if (player.isFrameNew()) {
        for (int j=0;j<player.getHeight();j++) {
            for (int i=0;i<player.getWidth();i++) {
                float brightness = player.getPixels().getColor(i, j).getBrightness();
                image.at<uchar>(j,i) = brightness>90 ? 0 : 255;
            }
        }
        
        vector<vector<cv::Point> > contours;
        cv::findContours(image, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE); // CV_CHAIN_APPROX_NONE
        vector<pair<int,float> > areas;
        int i=0;
        for (auto &c:contours) {
            areas.push_back(make_pair(i++, cv::contourArea(c)));
        }
        
        sort(areas.begin(),areas.end(),[](pair<int,float> &a,pair<int,float> &b) {
            return a>b;
        });
        
        contour.clear();
        if (!areas.empty()) {
            //cout << areas.front().second << endl;
            for (auto &p:contours[areas.front().first]) {
                contour.push_back(ofPoint(p.x,p.y));
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofNoFill();
    ofSetColor(ofColor::white);
    player.draw(0, 0);
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
