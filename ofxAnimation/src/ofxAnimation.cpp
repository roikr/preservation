//
//  ofxAnimation.cpp
//  backgroundPlayer
//
//  Created by Roee Kremer on 05/03/2017.
//
//

#include "ofxAnimation.h"


//--------------------------------------------------------------
void ofxAnimation::setup(){
    ofEnableAlphaBlending();
    bgFbo.allocate(ofGetWidth(),ofGetHeight());
    fgFbo.allocate(ofGetWidth(),ofGetHeight());
    fbo.allocate(ofGetWidth(),ofGetHeight());
    
    
    
    
    alpha.load("Light FG_a.mov");
    alpha.setLoopState(OF_LOOP_NORMAL);
    foreground.load("Light FG_rgb.mov");
    foreground.setLoopState(OF_LOOP_NORMAL);
    
    background.load("Light BG.mov");
    background.setLoopState(OF_LOOP_NORMAL);
    background.play();
    foreground.play();
    alpha.play();
    
    ofLoadImage(mask,"circle_mask.png");
    //
}

void ofxAnimation::update(){
    background.update();
    foreground.update();
    alpha.update();
    
    for (animation &player:players) {
        player.rgb.update();
        player.alpha.update();
    }
    
    if (background.isLoaded()) {
        bgFbo.begin();
        background.draw(0,0);
        bgFbo.end();
    }
    
    if (foreground.isLoaded() && alpha.isLoaded() ) {
        foreground.getTexture().setAlphaMask(alpha.getTexture());
        fgFbo.begin();
        ofClear(0,0,0,0);
        foreground.draw(0, 0);
        fgFbo.end();
        
    }
}

void ofxAnimation::draw() {
    bgFbo.draw(0,0);
    fbo.draw(0,0);
    fgFbo.draw(0,0);
    mask.draw(0,0);
}

void ofxAnimation::setBackground(int index) {
    switch (index) {
        case 0:
            alpha.setPaused(true);
            alpha.setPosition(0);
            background.load("Light BG.mov");
            foreground.load("Light FG_rgb.mov");
            background.play();
            foreground.play();
            alpha.setPaused(false);
            break;
        case 1:
            alpha.setPaused(true);
            alpha.setPosition(0);
            background.load("Normal BG.mov");
            foreground.load("Normal FG_rgb.mov");
            background.play();
            foreground.play();
            alpha.setPaused(false);
            break;
        case 2:
            alpha.setPaused(true);
            alpha.setPosition(0);
            background.load("Dark BG.mov");
            foreground.load("Dark BG_rgb.mov");
            background.play();
            foreground.play();
            alpha.setPaused(false);
            break;
    }
}

int ofxAnimation::add(string name) {
    players.push_back(animation());
    animation &player(players.back());
    player.rgb.load(name+"_rgb.mov");
    player.rgb.setLoopState(OF_LOOP_NONE);
    player.alpha.load(name+"_a.mov");
    player.alpha.setLoopState(OF_LOOP_NONE);
    player.rgb.play();
    player.alpha.play();
    return players.size()-1;
}
