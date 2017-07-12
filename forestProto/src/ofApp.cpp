#include "ofApp.h"

// 1 meter if 10 pixels
#define BOX2D_TO_OF_SCALE 50.0f

float of2b(float x) {return x/BOX2D_TO_OF_SCALE;}
float b2of(float x) {return x*BOX2D_TO_OF_SCALE;}

b2Vec2 of2b(ofVec2f v) {return 1/BOX2D_TO_OF_SCALE*b2Vec2(v.x,v.y);}
ofVec2f b2of(b2Vec2 v) {return BOX2D_TO_OF_SCALE*ofVec2f(v.x,v.y);}

ofVec2f touchToWorld(float x,float y) {return ofVec2f(x,ofGetHeight()-y);}

enum {
    TYPE_GROUND,
    TYPE_POLY,
    TYPE_USER
};

vector<ofVec2f> createBox(float width,float height) {
    vector<ofVec2f> verts;
    verts.push_back(0.5*ofVec2f(-width,-height));
    verts.push_back(0.5*ofVec2f(width,-height));
    verts.push_back(0.5*ofVec2f(width,height));
    verts.push_back(0.5*ofVec2f(-width,height));
    return verts;
}

vector<ofVec2f> createRandomPoly(float minRad,float maxRad) {
    vector<ofVec2f> verts;
    int n=6;
    float a = 2*M_PI/n;
    for (int i=0;i<n;i++) {
        float r=ofRandom(minRad, maxRad);
        float a=2*M_PI*i/n+M_PI/12;
        verts.push_back(ofVec2f(r*cos(a),r*sin(a)));
        
    }
    return verts;
}

bool isDynamic(int type) {
    return type==TYPE_POLY;
}

bool isSensor(int type) {
    return type==TYPE_USER;
}

instance::instance(b2World *world,int type,ofVec2f pos,vector<ofVec2f> poly):poly(poly),type(type) {
    
    b2BodyDef def;
    def.type = isDynamic(type) ? b2_dynamicBody : b2_staticBody;
    def.position= of2b(pos);
//    def.angle = 0;
    
    body = world->CreateBody(&def);
    body->SetUserData(this);
    
    for (int i=0;i<poly.size();i++) {
        b2Vec2 verts[3];
        vector<ofVec2f> vec{ofVec2f(0),poly[i],poly[(i+1)%poly.size()]};
        for (int j=0;j<3;j++) verts[j]=of2b(vec[j]);
        b2PolygonShape tri;
        tri.Set(verts, 3);
        b2FixtureDef fixture;
        fixture.density = 1;
        fixture.restitution = 0;
        fixture.friction = 1;
        fixture.shape = &tri;
        fixture.isSensor = isSensor(type);
        body->CreateFixture(&fixture);
    }
}

instance::instance(b2World *world,int type,ofVec2f pos,float width,float height):type(type) {
    
    poly = createBox(width,height);
    
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

//--------------------------------------------------------------
void ofApp::setup(){
//    ofSetWindowPosition(0, -ofGetHeight());
    ofSetBackgroundColor(ofColor::black);
    ofNoFill();
    
    m_world = new b2World(b2Vec2(0,-9.8f));
    m_world->SetContactListener(this);
    
    //instances.push_back(make_shared<instance>(m_world,TYPE_GROUND,false,createBox(ofGetWidth(),50),ofVec2f(ofGetWidth()/2,ofGetHeight())));
    instances.push_back(make_shared<instance>(m_world,TYPE_GROUND,ofVec2f(ofGetWidth()/2,0),ofGetWidth(),10));
}

//--------------------------------------------------------------
void ofApp::update(){
    int32 velocityIterations=8;
    int32 positionIterations=3;
    m_world->Step(1.0/60, velocityIterations, positionIterations);
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
    ofTranslate(0, ofGetHeight());
    ofScale(1,-1,1);
    ofSetColor(counter ? ofColor::red : ofColor::white);
    for (auto &i:instances) {
        ofPushMatrix();
        ofTranslate(b2of(i->body->GetPosition()));
        ofRotate(i->body->GetAngle()*180/M_PI);
        ofBeginShape();
        for (auto &v:i->poly) {
            ofVertex(v.x,v.y);
        }
        ofEndShape(true);
        ofPopMatrix();
    }
    ofPopMatrix();
}

void ofApp::exit(){
    delete m_world;
}

void ofApp::BeginContact(b2Contact* contact) {
    auto inA = (instance *)contact->GetFixtureA()->GetBody()->GetUserData();
    auto inB = (instance *)contact->GetFixtureB()->GetBody()->GetUserData();
    
    if (inA->type==TYPE_USER || inB->type==TYPE_USER) {
        counter++;
        
        auto poly = inA->type==TYPE_POLY ? inA : inB;
        auto user = inA->type==TYPE_USER ? inA : inB;
        
        b2Vec2 v= poly->body->GetWorldCenter()-user->body->GetWorldCenter();
        v.Normalize();
        float offset = ofVec2f(0,1).angle(b2of(v)) > 0 ? 0.1 : -0.1;
        auto body = poly->body;
        body->ApplyLinearImpulse(0.3*body->GetMass()*ofGetFrameRate()*v,body->GetWorldCenter()+b2Vec2(offset,0),true);
    }
}

void ofApp::EndContact(b2Contact* contact) {
    auto inA = (instance *)contact->GetFixtureA()->GetBody()->GetUserData();
    auto inB = (instance *)contact->GetFixtureB()->GetBody()->GetUserData();
    
    if (inA->type==TYPE_USER || inB->type==TYPE_USER) {
        counter--;
    }
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
    ofVec2f pos=touchToWorld(x,y);
    
    switch (button) {
        case 1: {
            auto it = find_if(instances.begin(), instances.end(),[](shared_ptr<instance> p) { return p->type==TYPE_USER;});
            if (it!=instances.end()) {
                m_world->DestroyBody((*it)->body);
                instances.erase(it);
            }
            instances.push_back(make_shared<instance>(m_world,TYPE_USER,pos,400, 40));
        }  break;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    ofVec2f pos=touchToWorld(x,y);
    
    switch (button) {
        case 0:
            instances.push_back(make_shared<instance>(m_world,TYPE_POLY,pos,30, 70));
            break;
        case 1:
            instances.push_back(make_shared<instance>(m_world,TYPE_USER,pos,400, 40));
            break;
        case 2:
            instances.push_back(make_shared<instance>(m_world,TYPE_POLY,pos,createRandomPoly(50, 100)));
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    switch (button) {
        case 1: {
            auto it = find_if(instances.begin(), instances.end(),[](shared_ptr<instance> p) { return p->type==TYPE_USER;});
            if (it!=instances.end()) {
                m_world->DestroyBody((*it)->body);
                instances.erase(it);
            }
        }  break;
    }
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
