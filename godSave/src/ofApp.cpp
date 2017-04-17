#include "ofApp.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 800
#define CAMERA_WIDTH 1920
#define CAMERA_HEIGHT 1080

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(ofColor::black);
    ofSetWindowShape(SCREEN_WIDTH, SCREEN_HEIGHT);
    ofSetWindowPosition(0, -SCREEN_HEIGHT);
    
    panel.setup("","settings.xml",10,100);
    kinect.open();
    panel.add(kinect.params);
    panel.add(threshold.set("threshold", 90, 0, 255));
    panel.add(scale.set("scale", 0.5));
    panel.add(offset.set("offset",ofVec2f(0,0)));
        

    panel.loadFromFile("settings.xml");
    
    grayImage.allocate(CAMERA_WIDTH,CAMERA_HEIGHT);
    grayBg.allocate(CAMERA_WIDTH,CAMERA_HEIGHT);
    grayDiff.allocate(CAMERA_WIDTH,CAMERA_HEIGHT);
    bLearnBg = true;
    
    box2d.init();
    box2d.setGravity(0,10);
    box2d.createGround(0,SCREEN_HEIGHT,SCREEN_WIDTH,SCREEN_HEIGHT);
    box2d.setFPS(30.0);
    
    mat.scale(scale,scale,1);
    mat.translate(ofVec3f(offset));
}

static bool shouldRemove(shared_ptr<ofxBox2dBaseShape>shape) {
    return !ofRectangle(0, -200, SCREEN_WIDTH, SCREEN_HEIGHT+400).inside(shape->getPosition());
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate(),0));
    // add some circles every so often
    if((int)ofRandom(0, 100) == 0) {
        shared_ptr<ofxBox2dCircle> circle = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
        circle->setPhysics(0.3, 0.75, 0.05);
        circle->setup(box2d.getWorld(), (SCREEN_WIDTH/2)+ofRandom(-20, 20), -20, ofRandom(10, 40));
        circles.push_back(circle);
    }
    
    // remove shapes offscreen
    ofRemove(circles, shouldRemove);
    // ofRemove(polyShapes, shouldRemove);
    
    kinect.update();
    if( kinect.isFrameNew() ){
        //texDepth[d].loadData( kinects[d]->getDepthPixels() );
        
        ofPixels &depthPixels(kinect.getDepthPixels());
        ofPixels &rgbPixels(kinect.getRgbPixels());
        
        for(int i = 0; i < depthPixels.size(); i++){
            rgbPixels[i*4+3]=(depthPixels[i]>0 && depthPixels[i]<255) ? 255 : 0;
        }
        
        texRGB.loadData( kinect.getRgbPixels() );
        grayImage.setFromPixels(depthPixels);
        
        if (bLearnBg) {
            grayBg=grayImage;
            bLearnBg=false;
        }
        
        grayDiff.absDiff(grayBg,grayImage);
        
        grayDiff.threshold(threshold);
        int size=CAMERA_WIDTH*CAMERA_HEIGHT;
        contourFinder.findContours(grayDiff, 0.01*size, 0.5*size, 5, false);
        
    
        polyShapes.clear();
        for (int i = 0; i < contourFinder.nBlobs; i++){
            shared_ptr<ofxBox2dPolygon> poly = shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon);
            for (ofVec3f &v:contourFinder.blobs[i].pts) {
                poly->addVertex(mat.preMult(v));
            }
            poly->setPhysics(0.0, 0.0, 0.0);
            poly->simplify(0.9);
            poly->triangulatePoly(50);
            poly->create(box2d.getWorld());
            polyShapes.push_back(poly);
        }
        
            
    }
    
    box2d.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
    ofMultMatrix(mat);
    
    if (texRGB.isAllocated()) {
        ofSetColor(ofColor::white);
        texRGB.draw(0,0);
    }
    ofPopMatrix();
    
    /*
    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].draw(0,0);
    }
     */
     
    
    // some circles :)
    ofFill();
    ofSetHexColor(0xc0dd3b);
    for (int i=0; i<circles.size(); i++) {
        
        circles[i]->draw();
    }
    
//    ofColor color;
//    color.setHex(0x444342);
    ofSetColor(ofColor::indianRed);
    
    for (int i=0; i<polyShapes.size(); i++) {
//        polyShapes[i]->draw();
        polyShapes[i]->ofPolyline::draw();
        
//        ofCircle(polyShapes[i]->getPosition(), 30);
    }
    

    
    
    
    //grayImage.draw(0,0);
    panel.draw();
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
    ofVec2f pos=ofVec2f(x,y);
    mat.translate(pos-lastPos);
    lastPos=pos;
    offset = mat.getTranslation();
    panel.saveToFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    lastPos=ofVec2f(x,y);
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

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY ) {

    ofVec2f pos(x,y);
    mat.translate(-pos);
    float scale=pow(1.01,scrollY);
    mat.scale(scale, scale, 1);
    mat.translate(pos);
    
    offset = mat.getTranslation();
    this->scale = mat.getScale().x;
    panel.saveToFile("settings.xml");
    
//    cout << scale << endl;
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
