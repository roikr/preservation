#pragma once

#include "ofMain.h"
#include "ofxKinectV2.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxElements.h"
#include "ofxBox2d.h"
#include "ofxAnimation.h"

struct instance: public ofxBox2dPolygon {
    instance(element &e):e(e) {};
    //    ~instance();
    element &e;
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
    
        shared_ptr<instance> instantiate(element &e);
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
    
        void mouseScrolled(int x, int y, float scrollX, float scrollY );

    ofxBox2d box2d; // ofxBox2d should be declared before the vector for proper destruction (destruxtion order)
    ofxElements elements;
    vector<shared_ptr<instance>> instances;
    ofxAnimation animation;
    
    ofxPanel panel;
    //ofxKinectV2 kinect;
    ofTexture texRGB;
    
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    
    ofxCvContourFinder 	contourFinder;
    bool bLearnBg;
    ofParameter<int> threshold;
    
    vector <shared_ptr<ofxBox2dPolygon> >	polyShapes;

    ofMatrix4x4 mat;
    ofParameter<ofVec2f> offset;
    ofParameter<float> scale;
    ofVec2f lastPos;
    
    
    		
};
