#pragma once

#include "ofMain.h"
#include <gst/gst.h>
#include "GLFW/glfw3.h"
#include <gst/gl/gstgldisplay.h>

class ofApp : public ofBaseApp,public ofThread {

	public:
		void setup();
		void update();
		void draw();

		void threadedFunction();

		void render(int texture);
    
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
    
  
    GstGLDisplay *gl_display;  
    ofTexture tex;
};
