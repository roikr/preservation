#pragma once

#include "ofMain.h"
#include "ofxKinectV2.h"
#include "ofxGui.h"
#include "Box2D.h"


class ofApp : public ofBaseApp{

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
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void mouseScrolled(int x, int y, float scrollX, float scrollY );

        ofxKinectV2 kinect;
        ofxPanel panel;

        b2World *m_world;

        ofMatrix4x4 mat;
        ofParameterGroup parameters;
        ofParameter<string> fps;
        ofParameter<ofVec2f> offset;
    	ofParameter<float> scale;
    	ofVec2f lastPos;
};
