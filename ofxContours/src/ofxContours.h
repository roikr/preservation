//
//  ofxContours.h
//  elementsContainer
//
//  Created by Roee Kremer on 06/03/2017.
//
//
#pragma once

#include "ofMain.h"

struct contour {
    string name;
    vector<ofPoint> blob;
    ofRectangle bb;
};



class ofxContours {
    
public:
    void draw(contour &c);
    
    void scan();
    void save();
    void load();
    
    vector<contour> contours;
    map<string,int> contoursMap;
};
