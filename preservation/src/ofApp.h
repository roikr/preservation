#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"

struct footage {
    string name;
    ofTexture tex;
};

struct visual {
    ofPoint pos;
    int index;
    float time;
};

struct element {
    string name;
    vector<ofPoint> contour;
    ofRectangle bb;
    ofTexture tex;
    ofTexture hit;
    bool bGood;
    int footageIndex;
};

class instance: public ofxBox2dPolygon {
public:
    
    instance(element &e):e(e) {};
    
    element &e;
    ofPoint center;
    int state;
    ofMatrix4x4 mat;
    
    ofVec2f hitPos;
    
};


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        void contactStart(ofxBox2dContactArgs &args);
        shared_ptr<instance> instantiate(element &e);

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
    
    ofxBox2d box2d; // ofxBox2d should be declared before the vector for proper destruction (destruxtion order)

    vector<element> elements;
    vector<footage> footages;
    vector<shared_ptr<instance>> instances;
    vector<visual> visuals;
    
    bool bRelease;
    
//    ofFbo fbo;
    ofTexture mask;
    ofTexture background;
    ofTexture foreground;
		
};
