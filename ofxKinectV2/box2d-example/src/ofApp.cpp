#include "ofApp.h"
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

//NOTE: if you are unable to connect to your device on OS X, try unplugging and replugging in the power, while leaving the USB connected.
//ofxKinectV2 will only work if the NUI Sensor shows up in the Superspeed category of the System Profiler in the USB section.

//On OS X if you are not using the example project. Make sure to add OpenCL.framework to the Link Binary With Library Build Phase 
//and change the line in Project.xcconfig to OTHER_LDFLAGS = $(OF_CORE_LIBS) $(OF_CORE_FRAMEWORKS) -framework OpenCL

//--------------------------------------------------------------
void ofApp::setup(){
    
    //Uncomment for verbose info from libfreenect2
    //ofSetLogLevel(OF_LOG_VERBOSE);

    ofBackground(30, 30, 30);
    ofSetFrameRate(60);
    kinect.setup();

    parameters.setName("settings");
    parameters.add(kinect.params);
    parameters.add(scale.set("scale", 0.5));
    parameters.add(offset.set("offset",ofVec2f(0,0)));

    panel.setup(parameters);
    fps.setName("fps");
    panel.add(fps);
    
    panel.loadFromFile("settings.xml");

    mat.scale(scale,scale,1);
    mat.translate(ofVec3f(offset));

    //m_world = new b2World(b2Vec2(0,-9.8f));

}

void contour2shapes(vector<ofPoint> &contour,vector<b2PolygonShape> &shapes) {
    ofPolyline poly;
    
    poly.addVertices(contour);
    vector <TriangleShape>  triangles;
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
        
        
        b2Vec2          verts[3];
        
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
            b2PolygonShape  shape;
            shape.Set(verts, 3);
            shapes.push_back(shape);
            
            
            
            
        }
        
    }
}

//--------------------------------------------------------------
void ofApp::update(){

    fps = ofToString(ofGetFrameRate());

    kinect.update();
    return;

    cout << "copy contours";

    kinect.lock();
    vector<vector<ofPoint>> contours=kinect.contours;
    kinect.unlock();
    cout << "\tdone - ";

    ofVec2f pos = ofVec2f(0.5*STAGE_WIDTH,0);
    cout << contours.size() << " contours (pts/shapes):";
        for (auto &c:contours) {
            vector<ofPoint> contour;
            for (auto &v:c) {
                ofPoint p=mat.preMult(ofPoint(v.x,v.y,0));
                contour.push_back(ofPoint(p.x,STAGE_HEIGHT-p.y)-pos);
            }

            vector<b2PolygonShape> shapes;
            cout << '\t' << c.size() << '/';
            //contour2shapes(contour,shapes);
            cout << shapes.size();

            /*
            b2BodyDef def;
            def.type = b2_staticBody;
            def.position= of2b(pos);
            //    def.angle = 0;
            
            b2Body *body = m_world->CreateBody(&def);
            
            for (auto &s:shapes) {
                b2FixtureDef    fixture;
                fixture.shape = &s;
                fixture.density = 1;
                fixture.restitution = 0;
                fixture.friction = 1;
                //fixture.isSensor = isSensor(type);
                body->CreateFixture(&fixture);
            }
            cout << " b";
            */

            //instances.push_back(make_shared<userInstance>(m_world,pos,contour));
        }
        cout << endl;
}

//--------------------------------------------------------------
void ofApp::draw(){
    //ofDrawBitmapString("ofxKinectV2: Work in progress addon.\nBased on the excellent work by the OpenKinect libfreenect2 team\n\n-Requires USB 3.0 port ( superspeed )\n-Requires patched libusb. If you have the libusb from ofxKinect ( v1 ) linked to your project it will prevent superspeed on Kinect V2", 10, 14);
	ofPushMatrix();
    ofMultMatrix(mat);
    kinect.draw();
    ofPopMatrix();
    panel.draw();

}

void ofApp::exit() {

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 's': {
            ofXml settings;
            settings.serialize(parameters);
            settings.save("settings.xml");
            } break;
        case 'e':
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
    ofXml settings;
    settings.serialize(parameters);
    settings.save("settings.xml");
        
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    lastPos=ofVec2f(x,y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY ) {
    
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

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
