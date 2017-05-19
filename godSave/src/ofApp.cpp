#include "ofApp.h"


#define CAMERA_WIDTH 1920
#define CAMERA_HEIGHT 1080

enum {
    STATE_FREE,
    STATE_HIT,
    STATE_ANIMATION,
    STATE_DONE
};

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(ofColor::black);
    
    //ofSetWindowPosition(0, -ofGetWidth());
    
    elements.load();
    animation.setup();

    
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
    box2d.createGround(0,ofGetHeight()-100,ofGetWidth(),ofGetHeight()-100);
    box2d.setFPS(30.0);
    
    ofAddListener(box2d.contactStartEvents, this, &ofApp::contactStart);
    
    mat.scale(scale,scale,1);
    mat.translate(ofVec3f(offset));
}

static bool shouldRemove(shared_ptr<ofxBox2dBaseShape>shape) {
    return !ofRectangle(0, -400, ofGetWidth(), ofGetHeight()+400).inside(shape->getPosition());
}

shared_ptr<instance> ofApp::instantiate(element &e) {
    //    shared_ptr<instance> in = shared_ptr<instance>(new instance(e));
    auto in = std::make_shared<instance>(e);
    in->addVertices(e.blob);
    //    for (ofPoint &p:e.blob) {
    //        in->addVertex(p+ofPoint(width,0));
    //    }
    in->setPhysics(0.5, 0.1, 0.1);
    //    e.poly->simplify(0.9);
    in->triangulatePoly(10);
    in->create(box2d.getWorld());
    
    in->setData(in.get());
    in->center=in->getPosition();
    in->state = STATE_FREE;
    
    vector<string> good{"helicopter","flower","acorn","onion","pincorn"};
    in->bGood = find(good.begin(),good.end(),in->e.name)!=good.end();
    
    return in;
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate())+'\t'+ofToString(instances.size()));
    // add some circles every so often
    
    if(int(ofRandom(0, 100)) == 0) {
        element &e(elements.elements[int(ofRandom(100)) % elements.elements.size()]);
        auto in = instantiate(e);
        in->setPosition(ofRandom(100, ofGetWidth()-100), 0);
        instances.push_back(in);
    }

    ofRemove(instances, shouldRemove);
    // ofRemove(polyShapes, shouldRemove);
    
    for (auto &in:instances) {
        
        in->mat.makeIdentityMatrix();
        in->mat.translate(-in->center);
        in->mat.rotate(in->getRotation(),0,0,1);
        in->mat.translate(in->getPosition());
        
        if (in->state==STATE_HIT) {
            auto &player = animation.players[in->animation];
            if (player.rgb.isPlaying() && player.alpha.isPlaying()) {
                if (in->bGood) {
                    in->hitPos = in->getPosition()-in->center;
                }
                in->state=STATE_ANIMATION;
            }
            
        }
    }

    
    
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
    
    animation.update();
    
    animation.fbo.begin();
    ofClear(0,0,0,0);
    
    ofPushMatrix();
    ofMultMatrix(mat);
    
    if (texRGB.isAllocated()) {
        ofSetColor(ofColor::white);
        texRGB.draw(0,0);
    }
    ofPopMatrix();
    
    for (auto &in:instances) {
        ofPushMatrix();
        
        switch(in->state) {
            case STATE_FREE:
            case STATE_HIT:
                ofMultMatrix(in->mat);
                in->e.tex.draw(0,0);
                break;
            case STATE_ANIMATION: {
                auto &player = animation.players[in->animation];
                player.rgb.getTexture().setAlphaMask(player.alpha.getTexture());
                if (in->bGood) {
                    ofTranslate(in->hitPos);
                } else {
                    ofMultMatrix(in->mat);
                }
                player.rgb.draw(0,0);
            } break;
        }
        ofPopMatrix();
    }
    
    animation.fbo.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    animation.draw();
    

//    ofSetColor(ofColor::indianRed);
//    
//    for (int i=0; i<polyShapes.size(); i++) {
////        polyShapes[i]->draw();
//        polyShapes[i]->ofPolyline::draw();
//        
////        ofCircle(polyShapes[i]->getPosition(), 30);
//    }
    

    
    
    
    //grayImage.draw(0,0);
    panel.draw();
}

//--------------------------------------------------------------
void ofApp::contactStart(ofxBox2dContactArgs &e) {
    if(e.a != NULL && e.b != NULL) {
        
        if(e.a->GetType() == b2Shape::e_edge) {
            auto in = (instance*)e.b->GetBody()->GetUserData();
            
            if (in->state==STATE_FREE) {
                in->state=STATE_HIT;
                cout << in->e.name << endl;
                in->animation=animation.add(in->bGood ? "plant" : in->e.name);
            }
        }
        
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    animation.setBackground(key-'1');
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
