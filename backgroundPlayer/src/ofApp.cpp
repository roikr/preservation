#include "ofApp.h"

#define STRINGIFY(A) #A

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableAlphaBlending();
    
    cout << ofIsGLProgrammableRenderer() << endl;
    
    animation.setup();
    
    
/*
#define IN in
#define OUT out
#define TEXTURE texture
    uniform mat4 projectionMatrix; uniform mat4 modelViewMatrix; uniform mat4 textureMatrix; uniform mat4 modelViewProjectionMatrix; IN vec4 position; IN vec2 texcoord; IN vec4 color; IN vec3 normal; OUT vec4 colorVarying; OUT vec2 texCoordVarying; OUT vec4 normalVarying; void main() { colorVarying = color; texCoordVarying = (textureMatrix*vec4(texcoord.x,texcoord.y,0,1)).xy; gl_Position = modelViewProjectionMatrix * position; }
#version 330
    
#define IN in
#define OUT out
#define TEXTURE texture
#define FRAG_COLOR fragColor
    out vec4 fragColor;
    uniform sampler2D src_tex_unit0; uniform sampler2D src_tex_unit1; uniform float usingTexture; uniform float usingColors; uniform vec4 globalColor; IN float depth; IN vec4 colorVarying; IN vec2 texCoordVarying; void main(){ FRAG_COLOR = vec4(TEXTURE(src_tex_unit0, texCoordVarying).rgb, TEXTURE(src_tex_unit1, texCoordVarying).r)* globalColor; }

    
    string vertexProgram = "#version 330\n" + string(STRINGIFY(
           uniform mat4 textureMatrix;
           uniform mat4 modelViewProjectionMatrix;
           in vec4 position;
           in vec2 texcoord;
           in vec4 color;
           
           out vec4 colorVarying;
           out vec2 texCoordVarying;
        
           void main() {
               colorVarying = color;
               texCoordVarying = (textureMatrix*vec4(texcoord.x,texcoord.y,0,1)).xy;
               gl_Position = modelViewProjectionMatrix * position;
           }
             ));

    
    string fragmentProgram = "#version 330\n" + string(STRINGIFY(
                                     uniform sampler2D tex0;
                                     uniform sampler2D maskTex;
                                     
                                     in vec2 texCoordVarying;
                                     out vec4 fragColor;
                                     
                                     void main (void){
                                         fragColor = texture(tex0, texCoordVarying);
                                     }));
    
//    shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexProgram);
//    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentProgram);
//    shader.linkProgram();
   */
}

//--------------------------------------------------------------
void ofApp::update(){
    animation.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    animation.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    animation.setBackground(key-'1');
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    animation.addAnimation((int)ofRandom(100)%animation.filenames.size(),0,ofPoint(x,y));
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
