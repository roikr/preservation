#pragma once

#include "ofMain.h"
#include "ofxContours.h"
#include "ofxBox2d.h"
#include "ofxAnimation.h"




class instance: public ofxBox2dPolygon {
public:
    instance(contour &c):c(c) {};
    //    ~instance();
    contour &c;
    ofPoint center;
    int state;
    ofMatrix4x4 mat;
    int animation;
    bool bGood;
    ofVec2f hitPos;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        shared_ptr<instance> instantiate(contour &c);
        void contactStart(ofxBox2dContactArgs &e);
            
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
    
    ofxContours contours;
    bool bRelease;
    
    
    
    ofxBox2d box2d; // ofxBox2d should be declared before the vector for proper destruction (destruxtion order)
    vector<shared_ptr<instance>> instances;
    
    ofxAnimation animation;
    
    
};
