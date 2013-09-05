#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxUI.h"

class ofxSyphonDoubler{

public:
    
    ofxSyphonDoubler();
    ~ofxSyphonDoubler();
    
    void setup();    
    void update(ofEventArgs &data);
    void draw();

    void setShow(bool);
    bool bShow;

    ofxSyphonServerDirectory dir;
    ofxSyphonClient client_1;
    ofxSyphonClient client_2;
    ofxSyphonServer server;
    void serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg);
    void serverRetired(ofxSyphonServerDirectoryEventArgs &arg);
    void setClientLeft(string name);
    void setClientRight(string name);

    int width, height;
    void setSize(int w, int h);
    bool bResize;
    
    ofxUICanvas *guiSize;
    ofxUICanvas *guiLeft;
    ofxUICanvas *guiRight;
    ofxUIDropDownList *ddlLeft;
    ofxUIDropDownList *ddlRight;
    void guiEvent(ofxUIEventArgs &e);
    vector<string> getInputAppNames();
    
protected:
    vector<string> names;
    ofFbo fbo;
    
};
