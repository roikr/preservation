#pragma once

#include "ofMain.h"
#include "Box2D.h"
#include "ofxKinectV2.h"
#include "ofxGui.h"

struct footage {
    string name;
    ofTexture tex;
};

struct visual {
    ofPoint pos;
    int index;
    bool bGood;
    float time;
};

struct element {
    element(string name,bool bGood,vector<ofPoint> &contour,ofRectangle bb,int footageIndex);
    
    string name;
    vector<ofPoint> contour;
    ofRectangle bb;
    ofTexture tex;
    ofTexture hit;
    bool bGood;
    int footageIndex;
    
    ofSoundPlayer sound;
    
    vector<b2PolygonShape> shapes;
};

class instance {
public:
    instance(int type):type(type) {};
    instance(b2World *world,int type,ofVec2f pos,float width,float height);
    
    b2Body *body;
    int type;
};


class polyInstance : public instance {
public:
    polyInstance(b2World *world,ofVec2f pos,element &e);
    
    element &e;
    int contact;
    float time;
};

class userInstance : public instance {
public:
    userInstance(b2World *world,ofVec2f pos,vector<ofPoint> &contour);

    vector<ofPoint> contour;
};


class ofApp : public ofBaseApp,b2ContactListener{

	public:
		void setup();
		void update();
		void draw();
    
        void removeUserInstances();
        void BeginContact(b2Contact* contact);
//        void EndContact(b2Contact* contact);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void mouseScrolled(int x, int y, float scrollX, float scrollY );
    

    b2World *m_world;

    vector<element> elements;
    vector<footage> footages;
    vector<shared_ptr<instance>> instances;
    vector<visual> visuals;
    
//    ofFbo fbo;
    ofTexture mask;
    vector<ofTexture> envs;
    
    ofxKinectV2 kinect;
    ofxPanel panel;
    
    ofMatrix4x4 mat;
    ofParameterGroup parameters;
    ofParameter<string> fps;
    ofParameter<float> margin;
    ofParameter<float> rate;
    ofParameter<ofVec2f> offset;
    ofParameter<float> scale;
    ofVec2f lastPos;
    
    bool bCalibrate;
    bool bManual;
    
    vector<ofSoundPlayer> goodSounds;
    vector<ofSoundPlayer> badSounds;
    
    int state;
    float instTime;
    
    //vector <shared_ptr<ofxBox2dPolygon> >	polyShapes;
    bool bHideMouse;
};
