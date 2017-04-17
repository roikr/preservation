#pragma once

#include "ofMain.h"
#include "ofxKinectV2.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxBox2d.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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

    
    
    ofxPanel panel;
    ofxKinectV2 kinect;
    ofTexture texRGB;
    
    ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage 	grayBg;
    ofxCvGrayscaleImage 	grayDiff;
    
    ofxCvContourFinder 	contourFinder;
    bool bLearnBg;
    ofParameter<int> threshold;
    
    ofxBox2d box2d;
    vector<shared_ptr<ofxBox2dCircle> > circles;
    vector <shared_ptr<ofxBox2dPolygon> >	polyShapes;

    ofMatrix4x4 mat;
    ofParameter<ofVec2f> offset;
    ofParameter<float> scale;
    ofVec2f lastPos;
    		
};
