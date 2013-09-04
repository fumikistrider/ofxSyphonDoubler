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

    width = ofGetWidth()*2;
    height = ofGetHeight();

    guiSize  = new ofxUICanvas();
    guiSize->addLabel("Output Size");
    guiSize->addSpacer();
    guiSize->addLabel("WIDTH");
    guiSize->addWidgetSouthOf(new ofxUITextInput(160,"text_width",ofToString(width),OFX_UI_FONT_SMALL), "WIDTH");
    guiSize->addWidgetEastOf(new ofxUITextInput(160,"text_height",ofToString(height),OFX_UI_FONT_SMALL), "text_width");
    guiSize->addWidgetNorthOf(new ofxUILabel("HEIGHT", OFX_UI_FONT_MEDIUM), "text_height");
    guiSize->autoSizeToFitWidgets();
    guiSize->setPosition(30, ofGetHeight()*0.75);
    ofAddListener(guiSize->newGUIEvent, this, &ofxSyphonDoubler::guiEvent);
    
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
    fbo.allocate(width,height,GL_RGBA);

    ofAddListener(ofEvents().update , this, &ofxSyphonDoubler::update );
    bShow = true;
    bResize = false;
    
}


//--------------------------------------------------------------
void ofxSyphonDoubler::setSize(int w, int h){
    
    bResize = true;
    width = w;
    height = h;
    fbo.allocate(width, height, GL_RGBA);
    bResize = false;
    
}

//--------------------------------------------------------------
void ofxSyphonDoubler::update(ofEventArgs &data){
    if(!bResize){
        fbo.begin();
        ofBackground(0, 0, 0, 0);
        ofSetColor(255, 255, 255);
        client_1.draw(0, 0, width*0.5, height);
        client_2.draw(width*0.5, 0, width*0.5, height);
        fbo.end();
        ofTexture tex = fbo.getTextureReference();
        server.publishTexture(&tex);
    }
}

//--------------------------------------------------------------
void ofxSyphonDoubler::draw(){

    if( bShow && !bResize){
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
    }else if( name == "text_width"){
        ofxUITextInput *textinput = (ofxUITextInput *) e.widget;
        if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER ||
           textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS){
            textinput->setAutoClear(false);
            width = ofToInt(textinput->getTextString());
            setSize(width, height);
        }
    }else if( name == "text_height"){
        ofxUITextInput *textinput = (ofxUITextInput *) e.widget;
        if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER ||
           textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS){
            textinput->setAutoClear(false);
            height = ofToInt(textinput->getTextString());
            setSize(width, height);
        }
    }
    
}

