#include "ofApp.h"
#include "ofxContours.h"

enum {
    STATE_FREE,
    STATE_CONTACT,
    STATE_GROUND,
    STATE_DONE
};


static bool shouldRemove(shared_ptr<instance>shape) {
    return !ofRectangle(0, -400, ofGetWidth(), ofGetHeight()+400).inside(shape->getPosition()) || (shape->state==STATE_GROUND && shape->e.bGood);
}

shared_ptr<instance> ofApp::instantiate(element &e) {
    //    shared_ptr<instance> in = shared_ptr<instance>(new instance(e));
    auto in = std::make_shared<instance>(e);
    
    in->addVertices(e.contour);
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
    
    return in;
}

//--------------------------------------------------------------
void ofApp::contactStart(ofxBox2dContactArgs &args) {
    if(args.a != NULL && args.b != NULL) {
        // newer element contact something
        if (args.b->GetBody()->GetUserData()) {
            auto in = (instance*)args.b->GetBody()->GetUserData();
            // other element
            if (args.a->GetBody()->GetUserData()) {
                switch (in->state) {
                    case STATE_FREE:
                        in->state=STATE_CONTACT;
                        break;
                    default:
                        break;
                }
            } else { // ground
                switch (in->state) {
                    case STATE_CONTACT:
                    case STATE_FREE: {
                        
                        visual v;
                        
                        v.index = in->e.footageIndex;
                        footage &f(footages[v.index]);
                        v.pos = worldPtToscreenPt(args.m.points[0]);
                        if (in->e.bGood) {
                            v.pos+=ofPoint(-0.5*f.tex.getWidth(),-f.tex.getHeight()+ofRandom(-30, 30));
                            
                        } else {
                            v.pos+=ofPoint(-0.5*f.tex.getWidth(),-f.tex.getHeight()+ofRandom(-10, 20));
                        }
                        
                        
                        v.time = ofGetElapsedTimef()+10;
                        visuals.push_back(v);
                        
                        in->state=STATE_GROUND;
                    } break;
                    default:
                        break;
                }
            }
        }
  
//        if(args.a->GetType() == b2Shape::e_edge) {
//            
//
//            
//        } else {
//            
//        }
        
        
        
    }
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetBackgroundAuto(false);
    ofSetWindowPosition(0, -ofGetHeight());
    
    
    vector<string> footagesInit={"plant","blue_spill","yellow_spill","garbage_spill","green_spill","spray_break"};
    for (auto &filename:footagesInit) {
        footages.push_back(footage());
        footage &f=footages.back();
        f.name = filename;
        ofLoadImage(f.tex,f.name+".png");
    }
    
    vector<pair<string,string>> debrisInit = {{"barrel","green_spill"},{"garbage","garbage_spill"},{"bottle","yellow_spill"},{"softner","blue_spill"},{"spray","blue_spill"}};
    
    vector<string> good{"helicopter","flower","acorn","onion","pincorn"};
    
    
    ofxContours contours;
    contours.load();
    
    for (auto &c:contours.contours) {
        bool bGood = find(good.begin(),good.end(),c.name)!=good.end();
        string fname;
        if (bGood) {
            fname="plant";
        } else {
            auto piter = find_if(debrisInit.begin(),debrisInit.end(),[c](const pair<string,string>&p ) { return c.name==p.first; });
            assert(piter!=debrisInit.end());
            fname=piter->second;
        }
        
        auto fiter=find_if(footages.begin(),footages.end(),[fname](const footage &f) {return fname==f.name;});
        assert(fiter!=footages.end());
        
        elements.push_back(element());
        element &e(elements.back());
        e.name=c.name;
        e.bGood = bGood;
        e.contour = c.blob;
        e.bb = c.bb;
        e.footageIndex=distance(footages.begin(),fiter);
        ofLoadImage(e.tex,e.name+".png");
        ofLoadImage(e.hit,e.name+"_hit.png");
    }
    
//    fbo.allocate(ofGetWidth(),ofGetHeight());
    ofLoadImage(mask,"mask.png");
    ofLoadImage(background,"background.png");
    ofLoadImage(foreground,"foreground.png");
    
    panel.setup("","settings.xml",10,100);
    kinect.setup();
    panel.add(kinect.params);
    //    panel.add(threshold.set("threshold", 90, 0, 255));
    panel.add(scale.set("scale", 0.5));
    panel.add(offset.set("offset",ofVec2f(0,0)));
    panel.loadFromFile("settings.xml");
    
    mat.scale(scale,scale,1);
    mat.translate(ofVec3f(offset));
    
    box2d.init();
    box2d.enableEvents();
    box2d.setGravity(0,10);
    box2d.createGround(0,ofGetHeight()-100,ofGetWidth(),ofGetHeight()-100);
    box2d.setFPS(60.0);
    
    // register the listener so that we get the events
    ofAddListener(box2d.contactStartEvents, this, &ofApp::contactStart);
    
    
    bRelease = true;

    
}



//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate())+'\t'+ofToString(instances.size()));
    
    kinect.update();
    
    kinect.lock();
    
    polyShapes.clear();
    for (int i = 0; i < 1/*contourFinder.nBlobs*/; i++){
        shared_ptr<ofxBox2dPolygon> poly = shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon);
        for (ofVec3f &v:kinect.contour/*contourFinder.blobs[i].pts*/) {
            poly->addVertex(mat.preMult(v));
        }
        poly->setPhysics(0.0, 0.0, 0.0);
        poly->simplify(0.9);
        poly->triangulatePoly(50);
        poly->create(box2d.getWorld());
        polyShapes.push_back(poly);
    }

    
    kinect.unlock();
    
    
    
    if(bRelease && int(ofRandom(0, 100)) == 0) {
        element &e(elements[int(ofRandom(100)) % elements.size()]);
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
    
    for (auto it=visuals.begin();it!=visuals.end();it++) {
        if (ofGetElapsedTimef()>it->time) {
            visuals.erase(it);
            break;
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofClear(0,0,0,0);
    
    
    
    background.draw(0,0);
    
    ofPushMatrix();
    ofMultMatrix(mat);
    kinect.draw();
    ofPopMatrix();
    
    //    ofSetColor(ofColor::indianRed);
    //
    //    for (int i=0; i<polyShapes.size(); i++) {
    ////        polyShapes[i]->draw();
    //        polyShapes[i]->ofPolyline::draw();
    //
    ////        ofCircle(polyShapes[i]->getPosition(), 30);
    //    }
    
    
    for (auto &v:visuals) {
        footages[v.index].tex.draw(v.pos);
    }
    
    
    for (auto &in:instances) {
        ofPushMatrix();
        ofMultMatrix(in->mat);
        
        if (in->e.bGood) {
            switch (in->state) {
                case STATE_FREE:
                case STATE_CONTACT:
                    //                ofBeginShape();
                    //                for (auto &p:in->e.contour) {
                    //                    ofVertex(p);
                    //                }
                    //                ofEndShape();
                    in->e.tex.draw(0,0);
                    break;
                default:
                    break;
            }
        } else {
        
            switch(in->state) {
                case STATE_FREE:
                    in->e.tex.draw(0,0);
                    break;
                case STATE_CONTACT:
                case STATE_GROUND:
                    in->e.hit.draw(0,0);
                    break;
                default:
                    break;
            }
        }
        ofPopMatrix();
    }
    
    
    
    mask.draw(0,0);
    foreground.draw(0,0);
    
    panel.draw();
    
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 't':
            ofToggleFullscreen();
            break;
        case ' ':
            kinect.exit();
            break;
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
