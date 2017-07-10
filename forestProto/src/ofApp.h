#pragma once

#include "ofMain.h"
#include "Box2D.h"



struct instance {
    instance(b2World *world,int type,ofVec2f pos,vector<ofVec2f> poly);
    instance(b2World *world,int type,ofVec2f pos,float width,float height);
    vector<ofVec2f> poly;
    int type;
    b2Body *body;
    
};

class ofApp : public ofBaseApp,b2ContactListener {

	public:
		void setup();
		void update();
		void draw();
        void exit();

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
    
        void BeginContact(b2Contact* contact);
        void EndContact(b2Contact* contact);
    
    
    b2World *m_world;
    
    vector<shared_ptr<instance>> instances;
    
    int counter;
    
};
