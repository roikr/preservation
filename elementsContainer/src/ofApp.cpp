#include "ofApp.h"

enum {
    STATE_FREE,
    STATE_HIT,
    STATE_ANIMATION,
    STATE_DONE
};

static bool shouldRemove(shared_ptr<ofxBox2dBaseShape>shape) {
    return !ofRectangle(0, -400, ofGetWidth(), ofGetHeight()+400).inside(shape->getPosition());
}
//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    ofSetWindowPosition(0, -ofGetHeight());
    //ofEnableAlphaBlending();
    
    contours.load();
    //animation.setup();
    animation.fbo.allocate(ofGetWidth(),ofGetHeight());
    
    
    box2d.init();
    box2d.enableEvents();
    box2d.setGravity(0,10);
    box2d.createGround(0,ofGetHeight()-200,ofGetWidth(),ofGetHeight()-100);
    box2d.setFPS(60.0);
    
    // register the listener so that we get the events
    ofAddListener(box2d.contactStartEvents, this, &ofApp::contactStart);
    
    
    bRelease = true;
}

shared_ptr<instance> ofApp::instantiate(contour &c) {
    //    shared_ptr<instance> in = shared_ptr<instance>(new instance(e));
    auto in = std::make_shared<instance>(c);
    
    in->addVertices(c.blob);
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
    in->bGood = find(good.begin(),good.end(),in->c.name)!=good.end();
    
    return in;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    ofSetWindowTitle(ofToString(ofGetFrameRate())+'\t'+ofToString(instances.size()));
    
    if(bRelease && int(ofRandom(0, 100)) == 0) {
        contour &e(contours.contours[int(ofRandom(100)) % contours.contours.size()]);
       auto in = instantiate(e);
       in->setPosition(ofRandom(100, ofGetWidth()-100), 0);
       instances.push_back(in);
    }
    
//    vector<string> filenames={"barrel","bottle","garbage","softner","helicopter","plant"};//,"bird","butterfly","hedghog"};

    ofVec2f mouse(ofGetMouseX(), ofGetMouseY());
    for (auto &in:instances) {
        
        in->mat.makeIdentityMatrix();
        in->mat.translate(-in->center);
        in->mat.rotate(in->getRotation(),0,0,1);
        in->mat.translate(in->getPosition());
        if (mouse.distance(in->getPosition())<100) {
            in->addRepulsionForce(mouse, 50);
        }
        
        
//        if (in->state==STATE_HIT) {
//            auto &player = animation.players[in->animation];
//            if (player.rgb.isPlaying() && player.alpha.isPlaying()) {
//                if (in->bGood) {
//                    in->hitPos = in->getPosition()-in->center;
//                }
//                in->state=STATE_ANIMATION;
//            }
//            
//        }
    }
    
    ofRemove(instances, shouldRemove);
    
    box2d.update();
//    animation.update();
    
    animation.fbo.begin();
    ofClear(0,0,0,0);
    
    
    for (auto &in:instances) {
        ofPushMatrix();
        
        switch(in->state) {
            case STATE_FREE:
            case STATE_HIT:
                ofMultMatrix(in->mat);
                contours.draw(in->c);
//                in->c.tex.draw(0,0);
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
    animation.fbo.draw(0,0);
}



//--------------------------------------------------------------
void ofApp::contactStart(ofxBox2dContactArgs &e) {
    if(e.a != NULL && e.b != NULL) {
        
        if(e.a->GetType() == b2Shape::e_edge) {
            
            auto in = (instance*)e.b->GetBody()->GetUserData();
            if (in->state==STATE_FREE) {
                cout << "apply force" << endl;
                in->state=STATE_HIT;
            }
            
//            if (in->state==STATE_FREE) {
//                in->state=STATE_HIT;
//                cout << in->e.name << endl;
//                in->animation=animation.add(in->bGood ? "plant" : in->e.name);
//                in->addForce(ofVec2f(0,10), 5);
//            }
                        //e.a->GetBody()->ApplyForceToCenter(<#const b2Vec2 &force#>, <#bool wake#>)
        }
        
        
        
    }
}




//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
//    animation.setBackground(key-'1');
    
//    if (key==' ') {
//        bRelease=!bRelease;
//    }
    
    for (auto &in:instances) {
        ofVec2f vel(in->getVelocity());
        
        in->addRepulsionForce(in->getPosition()+10*vel, 100);
        
//        in->mat.makeIdentityMatrix();
//        in->mat.translate(-in->center);
//        in->mat.rotate(in->getRotation(),0,0,1);
//        in->mat.translate(in->getPosition());
//        if (mouse.distance(in->getPosition())<100) {
//            in->addRepulsionForce(mouse, 50);
//        }
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
