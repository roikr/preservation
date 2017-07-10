#include "ofApp.h"

// 1 meter if 10 pixels
#define BOX2D_TO_OF_SCALE 50.0f

float of2b(float x) {return x/BOX2D_TO_OF_SCALE;}
float b2of(float x) {return x*BOX2D_TO_OF_SCALE;}

b2Vec2 of2b(ofVec2f v) {return b2Vec2(v.x/BOX2D_TO_OF_SCALE,(ofGetHeight()-v.y)/BOX2D_TO_OF_SCALE);}
ofVec2f b2of(b2Vec2 v) {return ofVec2f(v.x*BOX2D_TO_OF_SCALE,ofGetHeight()-v.y*BOX2D_TO_OF_SCALE);}

enum {
    TYPE_GROUND,
    TYPE_POLY,
    TYPE_USER
};

vector<ofVec2f> createBox(float width,float height) {
    vector<ofVec2f> verts;
    verts.push_back(0.5*ofVec2f(-width,height));
    verts.push_back(0.5*ofVec2f(width,height));
    verts.push_back(0.5*ofVec2f(width,-height));
    verts.push_back(0.5*ofVec2f(-width,-height));
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

instance::instance(b2World *world,int type,bool bDynamic,vector<ofVec2f> poly,ofVec2f pos):poly(poly),type(type) {
    
    b2BodyDef def;
    def.type = bDynamic ? b2_dynamicBody : b2_staticBody;
    def.position= of2b(pos);
//    def.angle = 0;
    
    body = world->CreateBody(&def);
    
    for (int i=0;i<poly.size();i++) {
        b2Vec2 verts[3];
        vector<ofVec2f> vec{ofVec2f(0),poly[i],poly[(i+1)%poly.size()]};
        for (int j=0;j<3;j++) verts[j].Set(of2b(vec[j].x),-of2b(vec[j].y));
        b2PolygonShape tri;
        tri.Set(verts, 3);
        b2FixtureDef fixture;
        fixture.density = 1;
        fixture.restitution = 0;
        fixture.friction = 1;
        fixture.shape = &tri;
        body->CreateFixture(&fixture);
    }
}

instance::instance(b2World *world,int type,bool bDynamic,float width,float height,ofVec2f pos):type(type) {
    
    poly = createBox(width,height);
    
    b2BodyDef def;
    def.type = bDynamic ? b2_dynamicBody : b2_staticBody;
    def.position= of2b(pos);
//    def.angle = 0;
    
    body = world->CreateBody(&def);
    
    b2PolygonShape box;
    box.SetAsBox(of2b(width)/2, of2b(height)/2);
    b2FixtureDef fixture;
    fixture.density = 1;
    fixture.restitution = 0;
    fixture.friction = 1;
    fixture.shape = &box;
    body->CreateFixture(&fixture);
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetBackgroundColor(ofColor::black);
    ofNoFill();
    
    m_world = new b2World(b2Vec2(0,-9.8f));
    
    //instances.push_back(make_shared<instance>(m_world,TYPE_GROUND,false,createBox(ofGetWidth(),50),ofVec2f(ofGetWidth()/2,ofGetHeight())));
    instances.push_back(make_shared<instance>(m_world,TYPE_GROUND,false,ofGetWidth(),10,ofVec2f(ofGetWidth()/2,ofGetHeight())));
}

//--------------------------------------------------------------
void ofApp::update(){
    int32 velocityIterations=8;
    int32 positionIterations=3;
    m_world->Step(1.0/60, velocityIterations, positionIterations);
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(ofColor::white);
    for (auto &i:instances) {
        ofPushMatrix();
        ofTranslate(b2of(i->body->GetPosition()));
        ofRotate(-i->body->GetAngle()*180/M_PI);
        ofBeginShape();
        for (auto &v:i->poly) {
            ofVertex(v.x,v.y);
        }
        ofEndShape(true);
        ofPopMatrix();
    }
}

void ofApp::exit(){
    delete m_world;
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
    switch (button) {
        case 0:
            instances.push_back(make_shared<instance>(m_world,TYPE_POLY,true,30, 70,ofVec2f(x,y)));
            break;
        case 2:
            instances.push_back(make_shared<instance>(m_world,TYPE_POLY,true,createRandomPoly(50, 100),ofVec2f(x,y)));
            break;
    }
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
