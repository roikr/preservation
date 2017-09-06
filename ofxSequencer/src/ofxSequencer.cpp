//
//  ofxSequencer.cpp
//  example
//
//  Created by Roee Kremer on 06/09/2017.
//

#include "ofxSequencer.h"

bool ofxSequencer::setup(string path,string prefix,int numDigits,int first,float framerate,float duration) {
    ofDirectory dir(path);
    size_t numFiles = dir.getFiles().size();
    
    if (!numFiles) {
        cout << "ofxSequencer: no file in " << path << endl;
        return false;
    }
    
    for (int i=0;i<numFiles;i++) {
        stringstream ss;
        ss << path << '/' << prefix << setw(numDigits) << setfill('0') << i+first << ".png";
        if (!ofFile(ss.str()).exists()) {
            cout << "ofxSequencer: missing " << ss.str() << endl;
            return false;
        }
    }
    
    textures.assign(numFiles, ofTexture());
    for (int i=0;i<numFiles;i++) {
        stringstream ss;
        ss << path << '/' << prefix << setw(numDigits) << setfill('0') << i+first << ".png";
        ofLoadImage(textures[i], ss.str());
    }
    
    this->framerate=framerate;
    this->duration = duration;
    return true;
}

void ofxSequencer::update() {
    float time = ofGetElapsedTimef();
    auto iter=instances.begin();
    while (iter!=instances.end() && time-iter->first<duration) {
        iter++;
    }
    
    instances.erase(iter, instances.end());
}

void ofxSequencer::draw() {
    
    float time = ofGetElapsedTimef();
    auto iter=instances.begin();
    while (iter!=instances.end() && MIN((time-iter->first)*framerate,textures.size()-1)) {
        iter++;
    }
    
    instances.erase(iter, instances.end());
    
    for (auto i:instances) {
        textures[MIN(textures.size()-1,(time-i.first)*framerate)].draw(i.second);
    }
}


void ofxSequencer::add(ofVec2f pos){
    instances.push_front(make_pair(ofGetElapsedTimef(),pos));
}
