//
//  ofxContours.cpp
//  elementsContainer
//
//  Created by Roee Kremer on 06/03/2017.
//
//

#include "ofxContours.h"
#include "ofxOpenCv.h"



//--------------------------------------------------------------
void ofxContours::scan() {
    
    ofDirectory dir(ofToDataPath(""));;
    dir.allowExt("png");
    dir.listDir();
    
    for (auto &f:dir.getFiles()) {
        //        cout << f.getBaseName() << endl;
        ofPixels pixels;
        ofLoadImage(pixels, f.path());
        
        ofPixels alphaPix;
        alphaPix.allocate(pixels.getWidth(), pixels.getHeight(), 1);
        //        cout << element.getWidth()*element.getHeight() << '\t' << element.getPixels().size() << endl;
        for (int i=0;i<pixels.size()/4;i++) {
            alphaPix.getData()[i]=pixels.getData()[i*4+3];
        }
        
        ofxCvContourFinder contourFinder;
        ofxCvGrayscaleImage 	grayImage;
        grayImage.allocate(pixels.getWidth(), pixels.getHeight());
        
        
        
        grayImage.setFromPixels(alphaPix);
        grayImage.threshold(100);
        
        int size = pixels.getWidth()*pixels.getHeight();
        contourFinder.findContours(grayImage, 0.1*size, size, 1, false);
        
        if (!contourFinder.blobs.empty()) {
            contours.push_back(contour());
            contour &c(contours.back());
            c.name = f.getBaseName();
            c.blob = contourFinder.blobs[0].pts;
            c.bb = contourFinder.blobs[0].boundingRect;
            contoursMap[c.name] = contours.size()-1;
            cout << c.name << '\t' << c.bb << endl;
            
        } else {
            cout << "can't trace " << f.getBaseName() << endl;
        }
    }
}

void ofxContours::save() {
    ofXml xml;
    xml.addChild("elements");
    xml.setTo("elements");
    for (auto &c:contours) {
        ofXml exml;
        exml.addChild("element");
        exml.setTo("element");
        exml.setAttribute("name", c.name);
        stringstream ss;
        ss << c.bb;
        exml.setAttribute("bb", ss.str());
        for (auto &p:c.blob) {
            ofXml pxml;
            pxml.addChild("point");
            pxml.setTo("point");
            pxml.setAttribute("x", ofToString(p.x,2));
            pxml.setAttribute("y", ofToString(p.y,2));
            exml.addXml(pxml);
        }
        xml.addXml(exml);
    }
    xml.save("elements.xml");
}

void ofxContours::load() {
    ofXml xml("elements.xml");
    if (xml.exists("element")) {
        xml.setTo("element[0]");
        do {
            contours.push_back(contour());
            contour &c(contours.back());
            c.name = xml.getValue("[@name]");
            stringstream ss;
            ss.str(xml.getValue("[@bb]"));
            ss >> c.bb;
            xml.setTo("point[0]");
            do {
                c.blob.push_back(ofPoint(xml.getValue<float>("[@x]"),xml.getValue<float>("[@y]")));
            } while (xml.setToSibling());
            xml.setToParent();
            contoursMap[c.name] = contours.size()-1;
        } while (xml.setToSibling());
    }
    
//    for (auto &e:elements) {
//        ofLoadImage(e.tex,e.name+".png");
//    }
}


void ofxContours::draw(contour &c) {
    ofBeginShape();
    for (auto &p:c.blob) {
        ofVertex(p);
    }
    ofEndShape();
}

