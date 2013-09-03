//
//  ofxSyphonDoubler
//

#include "ofxSyphonDoubler.h"


//--------------------------------------------------------------
ofxSyphonDoubler::ofxSyphonDoubler(){
    
}

//--------------------------------------------------------------
ofxSyphonDoubler::~ofxSyphonDoubler(){
    delete guiLeft;
    delete guiRight;
}

//--------------------------------------------------------------
void ofxSyphonDoubler::setup(){
    
    dir.setup();
    client_1.setup();
    client_2.setup();
    server.setName("ofxSyphonDoubler");
    ofAddListener(dir.events.serverAnnounced, this, &ofxSyphonDoubler::serverAnnounced);
    ofAddListener(dir.events.serverRetired, this, &ofxSyphonDoubler::serverRetired);
    
    guiLeft  = new ofxUICanvas();
    guiRight = new ofxUICanvas();
    ddlLeft  = guiLeft->addDropDownList("Left Input", names);
    ddlRight = guiRight->addDropDownList("Right Input", names);
    ddlLeft->setAllowMultiple(false);
    ddlRight->setAllowMultiple(false);
    guiLeft->autoSizeToFitWidgets();
    guiRight->autoSizeToFitWidgets();
    guiLeft->setPosition(0, ofGetHeight() * 0.5);
    guiRight->setPosition(ofGetWidth() * 0.5, ofGetHeight() * 0.5);
    ofAddListener(guiLeft->newGUIEvent, this, &ofxSyphonDoubler::guiEvent);
    ofAddListener(guiRight->newGUIEvent, this, &ofxSyphonDoubler::guiEvent);
    fbo.allocate(ofGetWidth()*2,ofGetHeight(),GL_RGBA);

    ofAddListener(ofEvents().update , this, &ofxSyphonDoubler::update );
    bShow = true;
    
}

//--------------------------------------------------------------
void ofxSyphonDoubler::update(ofEventArgs &data){
    
    fbo.begin();
    ofBackground(0, 0, 0, 0);
    ofSetColor(255, 255, 255);
    client_1.draw(0, 0, ofGetWidth(), ofGetHeight());
    client_2.draw(ofGetWidth(), 0, ofGetWidth(), ofGetHeight());
    fbo.end();
    ofTexture tex = fbo.getTextureReference();
    server.publishTexture(&tex);
    
}

//--------------------------------------------------------------
void ofxSyphonDoubler::draw(){

    if( bShow ){
        ofColor(255, 255, 255, 255);
        ofEnableAlphaBlending();
        
        ofTexture tex = fbo.getTextureReference();    
        tex.draw(0, 0, ofGetWidth(), ofGetHeight()*0.5);
    }
    
}

//--------------------------------------------------------------
void ofxSyphonDoubler::setShow(bool b){
    bShow = b;
}

//--------------------------------------------------------------
void ofxSyphonDoubler::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        names.push_back(dir.appName);
        ddlLeft->addToggle(dir.appName);
        ddlRight->addToggle(dir.appName);
    }
}

//--------------------------------------------------------------
void ofxSyphonDoubler::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        for(int i = 0; i < names.size(); i++){
            if( names[i] == dir.appName ) names.erase(names.begin() + i);
        }
        ddlLeft->removeToggle(dir.appName);
        ddlRight->removeToggle(dir.appName);
    }
}

//--------------------------------------------------------------
void ofxSyphonDoubler::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    
    // select input
    if( name == "Left Input" || name == "Right Input"){
        ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
        vector<ofxUIWidget*> selected = ddlist->getSelected();
        for(int i = 0; i < names.size(); i++){
            for( int k = 0; k < selected.size(); k++){
                if( names[i] == selected[0]->getName()){
                    if( name == "Left Input"){
                        client_1.set( dir.getDescription(i) );
                    }else if( name == "Right Input"){
                        client_2.set( dir.getDescription(i) );
                    }
                }
            }
        }
    }
}

