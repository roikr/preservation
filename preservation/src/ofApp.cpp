#include "ofApp.h"
#include "ofxContours.h"
#include "ofxBox2dPolygonUtils.h"



// 1 meter if 10 pixels
#define BOX2D_TO_OF_SCALE 50.0f
#define STAGE_WIDTH 1024
#define STAGE_HEIGHT 800

float of2b(float x) {return x/BOX2D_TO_OF_SCALE;}
float b2of(float x) {return x*BOX2D_TO_OF_SCALE;}

b2Vec2 of2b(ofVec2f v) {return 1/BOX2D_TO_OF_SCALE*b2Vec2(v.x,v.y);}
ofVec2f b2of(b2Vec2 v) {return BOX2D_TO_OF_SCALE*ofVec2f(v.x,v.y);}

ofVec2f touchToWorld(float x,float y) {return ofVec2f(x,STAGE_HEIGHT-y);}

enum {
    TYPE_GROUND,
    TYPE_POLY,
    TYPE_USER
};

enum {
    CONTACT_NONE,
    CONTACT_USER,
    CONTACT_GROUND
};


void contour2shapes(vector<ofPoint> &contour,vector<b2PolygonShape> &shapes) {
    ofPolyline poly;
    
    poly.addVertices(contour);
    vector <TriangleShape>	triangles;
    bool bIsSimplified = false;
    float resampleAmt = 10; // 20
    int nPointsInside=-1;
    
    triangles.clear();
    bool wasClosed = poly.isClosed();
    if(poly.size() > 0) {
        
        // copy over the points into a polyline
        ofPolyline polyOutline;
        ofPolyline newPoly;
        
        // make sure to close the polyline and then
        // simplify and resample by spacing...
        poly.setClosed(true);
        if(!bIsSimplified) poly.simplify();
        newPoly = poly.getResampledBySpacing(resampleAmt);
        
        // save the outline...
        polyOutline = newPoly;
        
        // add some random points inside then
        // triangulate the polyline...
        if(nPointsInside!=-1) addRandomPointsInside(newPoly, nPointsInside);
        triangles = triangulatePolygonWithOutline(newPoly, polyOutline);
        
        /*
        poly.clear();
        if(wasClosed) poly.setClosed(wasClosed);
        
        // now add back into polyshape
        for (size_t i=0; i<newPoly.size(); i++) {
            poly.addVertex(newPoly[i]);
        }
         */
    }
    
    for (auto &t:triangles) {
        
        
        b2Vec2			verts[3];
        
        vector<ofVec2f> vec{t.a,t.b,t.c};
        for (int j=0;j<3;j++) verts[j]=of2b(vec[j]);
        
        
        bool bPolyDeg=false;
        for (size_t j=0;j<3;j++) {
            for (size_t k=0;k<j;k++) {
                if (b2DistanceSquared(verts[j], verts[k]) < 0.5f * b2_linearSlop) {
                    bPolyDeg = true;
                    break;
                }
            }
        }
        
        if (!bPolyDeg) {
            b2PolygonShape	shape;
            shape.Set(verts, 3);
            shapes.push_back(shape);
            
            
            
            
        }
        
    }
}

element::element(string name,bool bGood,vector<ofPoint> &contour,ofRectangle bb,int footageIndex):name(name),bGood(bGood),bb(bb),footageIndex(footageIndex) {
    
    for (auto &v:contour) {
        this->contour.push_back(ofPoint(v.x,-v.y,0));
    }
    contour2shapes(this->contour,shapes);
    
    ofLoadImage(tex,name+".png");
    ofLoadImage(hit,name+"_hit.png");
}

bool isDynamic(int type) {
    return type==TYPE_POLY;
}

bool isSensor(int type) {
    return type==TYPE_USER;
}


instance::instance(b2World *world,int type,ofVec2f pos,float width,float height):type(type) {
    
    b2BodyDef def;
    def.type = isDynamic(type) ? b2_dynamicBody : b2_staticBody;
    def.position= of2b(pos);
    //    def.angle = 0;
    
    body = world->CreateBody(&def);
    body->SetUserData(this);
    
    b2PolygonShape box;
    box.SetAsBox(of2b(width)/2, of2b(height)/2);
    b2FixtureDef fixture;
    fixture.density = 1;
    fixture.restitution = 0;
    fixture.friction = 1;
    fixture.shape = &box;
    fixture.isSensor = isSensor(type);
    body->CreateFixture(&fixture);
}


polyInstance::polyInstance(b2World *world,ofVec2f pos,element &e):e(e),instance(TYPE_POLY),contact(CONTACT_NONE) {
    
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position= of2b(pos);
    //    def.angle = 0;
    
    body = world->CreateBody(&def);
    
    for (auto &s:e.shapes) {
        b2FixtureDef	fixture;
        fixture.shape = &s;
        fixture.density = 1;
        fixture.restitution = 0.05;
        fixture.friction = 0.95;
        body->CreateFixture(&fixture);
    }
    
    body->SetUserData(this);
    time=ofGetElapsedTimef();
   
}

userInstance::userInstance(b2World *world,ofVec2f pos,vector<ofPoint> &contour):instance(TYPE_USER),contour(contour) {
    
    

   // for (int i = 0; i < 1/*contourFinder.nBlobs*/; i++){
   //     shared_ptr<ofxBox2dPolygon> poly = shared_ptr<ofxBox2dPolygon>(new ofxBox2dPolygon);
   //     for (ofVec3f &v:kinect.contour/*contourFinder.blobs[i].pts*/) {
   //         poly->addVertex(mat.preMult(v));
   //     }
    
   // }

    

    vector<b2PolygonShape> shapes;
    contour2shapes(contour,shapes);

    b2BodyDef def;
    def.type = b2_staticBody;
    def.position= of2b(pos);
    //    def.angle = 0;
    
    body = world->CreateBody(&def);
    
    for (auto &s:shapes) {
        b2FixtureDef    fixture;
        fixture.shape = &s;
        fixture.density = 1;
        fixture.restitution = 0;
        fixture.friction = 1;
        fixture.isSensor = isSensor(type);
        body->CreateFixture(&fixture);
    }
    
    
    body->SetUserData(this);
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetBackgroundAuto(false);
    ofDisableArbTex();
    //ofDisableTextureEdgeHack();
    //ofSetWindowPosition(0, 0);//-ofGetHeight());
    ofSetFrameRate(60);

    gst_init(NULL,NULL);
    

#ifndef NO_KINECT
    if (!kinect.setup()) {
        std::exit(EXIT_FAILURE);
    }
#endif
    

    ofDirectory dir("sounds/good");
    dir.listDir();
    for (auto f:dir) {
        ofSoundPlayer s;
        s.load(f.path());
        goodSounds.push_back(s);
    }
    
    dir.listDir("sounds/bad");
    for (auto f:dir) {
        cout << f.path() << endl;
        ofSoundPlayer s;
        s.load(f.path());
        badSounds.push_back(s);
    }
    
    vector<string> footagesInit={"plant","blue_spill","yellow_spill","garbage_spill","green_spill","spray_break"};
    for (auto &filename:footagesInit) {
        footages.push_back(footage());
        footage &f=footages.back();
        f.name = filename;
        ofLoadImage(f.tex,f.name+".png");
    }
    
    vector<pair<string,string>> debrisInit = {{"barrel","green_spill"},{"garbage","garbage_spill"},{"bottle","yellow_spill"},{"softner","blue_spill"},{"spray","blue_spill"}};
    
    vector<string> good{"helicopter","flower","acorn","onion","pincorn"};
    
    ofXml sounds("sounds/sounds.xml");
    
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
        
        elements.push_back(element(c.name,bGood,c.blob,c.bb,distance(footages.begin(),fiter)));
        element &e=elements.back();
        cout << e.name << ": " << e.shapes.size() << endl;
        if (sounds.exists("//sound[@name='"+e.name+"']")) {
            sounds.setTo("//sound[@name='"+e.name+"']");
            string sname = sounds.getValue("[@filename]");
            cout << "sound: " << sname << endl;
            e.sound.load("sounds/"+sname);
            sounds.setTo("//");
        }
    }
    
    ofLoadImage(mask,"mask.png");
    
    vector<string> textures{"bg_dark","fg_dark","bg_normal","fg_normal","bg_light","fg_light"};
    for (auto &f:textures) {
        ofTexture tex;
        ofLoadImage(tex,f+".png");
        envs.push_back(tex);
    }
    
    plant.setup("plant","plant grow_",5,0,25,10);
    
    parameters.setName("settings");
#ifndef NO_KINECT
    parameters.add(kinect.params);
#endif
    parameters.add(margin.set("margin",200,0,400));
    //    parameters.add(threshold.set("threshold", 90, 0, 255));
    parameters.add(scale.set("scale", 0.5));
    parameters.add(offset.set("offset",ofVec2f(0,0)));
    parameters.add(rate.set("rate",1.5,1,5));
    
    panel.setup(parameters);
    fps.setName("fps");
    panel.add(fps);
    bodies.setName("bodies");
    panel.add(bodies);
    panel.loadFromFile("settings.xml");
    
    mat.scale(scale,scale,1);
    mat.translate(ofVec3f(offset));
    
    m_world = new b2World(b2Vec2(0,-9.8f));
    m_world->SetContactListener(this);
    
    instances.push_back(make_shared<instance>(m_world,TYPE_GROUND,ofVec2f(STAGE_WIDTH/2,100),STAGE_WIDTH,10));
    
//    box2d.setFPS(60.0);
    
    bCalibrate = false;
    bManual=false;
    state=1;
    bVideoStarted = false;
    stateChanged = instTime = ofGetElapsedTimef();
    bHideMouse=true;
}

void ofApp::removeUserInstances() {
    instances.erase(remove_if(instances.begin(),instances.end(),[this](shared_ptr<instance> i) {
        if (i->type==TYPE_USER) {
            auto user = static_pointer_cast<userInstance>(i);
            user->contour.clear();
            m_world->DestroyBody(i->body);
            return true;
        } else {
            return false;
        }
    }),instances.end());
    
    /*
     auto it = find_if(instances.begin(), instances.end(),[](shared_ptr<instance> p) { return p->type==TYPE_USER;});
     if (it!=instances.end()) {
     m_world->DestroyBody((*it)->body);
     instances.erase(it);
     }
     */
}


//--------------------------------------------------------------
void ofApp::update(){
    fps = ofToString(ofGetFrameRate(),0);
    bodies = ofToString(m_world->GetBodyCount());
    if (bHideMouse && ofGetElapsedTimef()>5) {
        bHideMouse = false;
        ofHideCursor();
    }
    

    //ofSetWindowTitle("fps: "+ofToString(ofGetFrameRate())+"\tinstances: "+ofToString(instances.size())+"\tvisuals: " + ofToString(visuals.size()));
    

#ifndef NO_KINECT
    if (!bManual) {

        if (!kinect.update()) {
            std::exit(EXIT_FAILURE);
        }
        
        kinect.lock();
        vector<vector<ofPoint>> contours=kinect.contours;
        kinect.unlock();

        removeUserInstances();
        
        ofVec2f pos = ofVec2f(0.5*STAGE_WIDTH,0);
        for (auto &c:contours) {
            vector<ofPoint> contour;
            for (auto &v:c) {
                ofPoint p=mat.preMult(ofPoint(v.x,v.y,0));
                contour.push_back(ofPoint(p.x,STAGE_HEIGHT-p.y)-pos);
            }
           instances.push_back(make_shared<userInstance>(m_world,pos,contour));
        }
    }
#endif
    
    instances.erase(remove_if(instances.begin(),instances.end(),[this](shared_ptr<instance> i) {
        switch (i->type) {
            case TYPE_POLY: {
                //!ofRectangle(0, -400, ofGetWidth(), ofGetHeight()+400).inside(shape->getPosition()) || (shape->state==STATE_GROUND && shape->e.bGood);
                auto poly = static_pointer_cast<polyInstance>(i);
                if  (poly->body->IsAwake() && ofGetElapsedTimef()<poly->time+5) {
                    ofVec2f pos = b2of(i->body->GetPosition());
                    if (pos.x<-100 || pos.x>STAGE_WIDTH+100) {
                        m_world->DestroyBody(poly->body);
                        return true;
                    }
                } else {
                    m_world->DestroyBody(poly->body);
                    return true;
                }
            }    break;
            default:
                break;
        }
        return false;
    }),instances.end());
    
    
    if (ofGetElapsedTimef() > instTime+rate*(1+ofRandomf()/5)) { // if(int(ofRandom(0, 100)) == 0) {
        element &e(elements[int(ofRandom(100)) % elements.size()]);
        instances.push_back(make_shared<polyInstance>(m_world,ofVec2f(ofRandom(margin, STAGE_WIDTH-margin),STAGE_HEIGHT),e));
        instTime = ofGetElapsedTimef();
    }
    
    
    int32 velocityIterations=8;
    int32 positionIterations=3;
    m_world->Step(1.0/60, velocityIterations, positionIterations);
    
    
    
    for (auto it=visuals.begin();it!=visuals.end();it++) {
        if (ofGetElapsedTimef()>it->time) {
            visuals.erase(it);
            break;
        }
    }
    
    
    int last = state;
    if (visuals.size()) {
        state = visuals.size()>plant.numInstances() ? 0 : 1;
    } else if(plant.numInstances()>3) {
        state = 2;
    }
    
    if (state!=last) {
        stateChanged = ofGetElapsedTimef();
    }
    
    
    if (state==2) {
        if (ofGetElapsedTimef()-stateChanged>5) {
            
            if (!foreground.isPlaying() && !alpha.isPlaying() && plant.numInstances()>3 ) {
                vector<string> animals={"butterfly","bird","hedgehog"};
                vector<pair<int,int> > ranges={make_pair(250, 450),make_pair(0, 250),make_pair(550, 650)};
                
                int animal=rand()%3;
                animalPos = ofRandom(ranges[animal].first, ranges[animal].second);
                
                foreground.setup("filesrc location="+ofToDataPath("videos/"+animals[animal]+"_rgb.mov")+" ! qtdemux ! h264parse ! vaapidecode ! glimagesink sync=1 name=video",{"video"},false);
                
                alpha.setup("filesrc location="+ofToDataPath("videos/"+animals[animal]+"_a.mov")+" ! qtdemux ! h264parse ! vaapidecode ! glimagesink sync=1 name=video",{"video"},false);
            }
        }
    } else {
        if (foreground.isPlaying()) {
            foreground.exit();
        }
        if (alpha.isPlaying()) {
            alpha.exit();
        }
    }
    
    
    // foreground.update();
    // alpha.update();
    
    //cout << counter << '/' << visuals.size() << '\t' << state << endl;
    plant.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofClear(0,0,0,0);
    
    envs[state*2].draw(0,0);
    
#ifndef NO_KINECT
    ofPushMatrix();
    ofMultMatrix(mat);
    kinect.draw();
    ofPopMatrix();
#endif
    
    plant.draw();
    
    for (auto &v:visuals) {
        footages[v.index].tex.draw(v.pos);
    }
    
    ofPushMatrix();
    ofTranslate(0, STAGE_HEIGHT);
    ofScale(1,-1,1);
    ofSetColor(ofColor::white);
    for (auto &i:instances) {
        ofPushMatrix();
        ofTranslate(b2of(i->body->GetPosition()));
        ofRotate(i->body->GetAngle()*180/M_PI);
        switch (i->type) {
            case TYPE_USER: {
                if (bManual) {
                    ofRectangle rect;
                    rect.setFromCenter(0, 0, 400, 40);
                    ofDrawRectangle(rect);
                } 

                // ofFill();
                // auto user = static_pointer_cast<userInstance>(i);
                
                
                // ofBeginShape();
                // for (auto &v:user->contour) {
                //     ofVertex(v.x,v.y);
                // }
                // ofEndShape(true);
                // ofNoFill();

            } break;
            case TYPE_POLY: {
                auto poly = static_pointer_cast<polyInstance>(i);
                element &e = poly->e;

                ofPushMatrix();
                ofScale(1,-1,1);
                if (poly->contact==CONTACT_GROUND && !poly->e.bGood) {
                    e.hit.draw(0,0);
                } else {
                    e.tex.draw(0,0);
                }
                ofPopMatrix();

            } break;
        }
        
        ofPopMatrix();
    }
    ofPopMatrix();

    
    if (foreground.isPlaying() && foreground.getTextures()[0].isAllocated() && alpha.getTextures()[0].isAllocated()) {
        foreground.getTextures()[0].setAlphaMask(alpha.getTextures()[0]);
        foreground.getTextures()[0].draw(0,animalPos);
    }
    
    mask.draw(0,0);
        
    envs[state*2+1].draw(0,0);
    
    if (bCalibrate) {
        panel.draw();
    }
    
}


bool checkTypes(instance *inA,instance *inB,int type1,int type2) {
    return (inA->type==type1 && inB->type==type2) || (inA->type==type2 && inB->type==type1);
}


void ofApp::BeginContact(b2Contact* contact) {
    auto inA = (instance *)contact->GetFixtureA()->GetBody()->GetUserData();
    auto inB = (instance *)contact->GetFixtureB()->GetBody()->GetUserData();
    
    if (checkTypes(inA,inB,TYPE_POLY,TYPE_USER)) {

        auto poly = static_cast<polyInstance*>(inA->type==TYPE_POLY ? inA : inB);

        if (poly->contact==CONTACT_NONE) {
            poly->contact=CONTACT_USER;
            auto user = inA->type==TYPE_USER ? inA : inB;
            
            b2Vec2 v= poly->body->GetWorldCenter()-user->body->GetWorldCenter();
            v.Normalize();
            //cout << b2of(poly->body->GetWorldCenter()) << '\t' << b2of(user->body->GetWorldCenter()) << '\t' << b2of(user->body->GetPosition()) <<endl;
            float offset = ofVec2f(0,1).angle(b2of(v)) > 0 ? 0.1 : -0.1;
            auto body = poly->body;
            body->ApplyLinearImpulse(0.5*body->GetMass()*ofGetFrameRate()*v,body->GetWorldCenter()+b2Vec2(offset,0),true);
            
            if (poly->e.bGood) {
                if (!goodSounds.empty()) {
                    goodSounds[rand()%goodSounds.size()].play();
                }
            } else {
                if (!badSounds.empty()) {
                    badSounds[rand()%badSounds.size()].play();
                }
            }
        }
        
    }
    
    if (checkTypes(inA,inB,TYPE_POLY,TYPE_GROUND)) {
        //counter++;
        
        auto poly = static_cast<polyInstance*>(inA->type==TYPE_POLY ? inA : inB);
        
        if (poly->contact==CONTACT_NONE) {
            poly->contact=CONTACT_GROUND;
            element &e = (poly)->e;
            
            b2WorldManifold manifold;
            contact->GetWorldManifold(&manifold);
            ofVec2f pos = b2of(manifold.points[0]);
            
            if (e.bGood) {
                plant.add(ofPoint(pos.x-0.5*plant.getWidth(),STAGE_HEIGHT-pos.y-plant.getHeight()+ ofRandom(-30, 30)));
            } else {
                visual v;
                v.index = e.footageIndex;
                footage &f(footages[v.index]);
                v.pos=ofPoint(pos.x-0.5*f.tex.getWidth(),STAGE_HEIGHT-pos.y-f.tex.getHeight()+ofRandom(-10, 20));
                v.time = ofGetElapsedTimef()+10;
                visuals.push_back(v);
            }
            
            if (poly->e.sound.isLoaded()) {
                poly->e.sound.play();
            }
            
        }
        
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    
    switch(key) {
        case 'm':
            bManual=!bManual;
            removeUserInstances();
            break;
#ifndef NO_KINECT
        case 'e':
            kinect.exit();
            break;
#endif
        case 't':
            ofToggleFullscreen();
            break;
        case 'c':
            bCalibrate=!bCalibrate;
            if (bCalibrate) {
                panel.registerMouseEvents();
                ofShowCursor();
            } else {
                panel.unregisterMouseEvents();
                ofHideCursor();
            }
            break;
        case 's': {
            ofXml settings;
            settings.serialize(parameters);
            settings.save("settings.xml");
        }
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
    if (bCalibrate) {
        ofVec2f pos=ofVec2f(x,y);
        mat.translate(pos-lastPos);
        lastPos=pos;
        offset = mat.getTranslation();
        ofXml settings;
        settings.serialize(parameters);
        settings.save("settings.xml");
        
    }
    
    if (bManual) {
        ofVec2f pos=touchToWorld(x,y);
        removeUserInstances();
        instances.push_back(make_shared<instance>(m_world,TYPE_USER,pos,400, 40));
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    if (bCalibrate) {
        lastPos=ofVec2f(x,y);
    }
    
    if (bManual) {
        ofVec2f pos=touchToWorld(x,y);
        instances.push_back(make_shared<instance>(m_world,TYPE_USER,pos,400, 40));
    }

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    if (bManual) {
        ofVec2f pos=touchToWorld(x,y);
        removeUserInstances();
    }

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY ) {
    if (bCalibrate) {
        ofVec2f pos(x,y);
        mat.translate(-pos);
        float scale=pow(1.01,scrollY);
        mat.scale(scale, scale, 1);
        mat.translate(pos);
        
        offset = mat.getTranslation();
        this->scale = mat.getScale().x;
        ofXml settings;
        settings.serialize(parameters);
        settings.save("settings.xml");
    }
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
