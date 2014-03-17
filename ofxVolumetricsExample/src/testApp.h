#pragma once

#include "ofMain.h"
#include "ofxVolumetrics.h"

class testApp : public ofBaseApp
{

public:
    void setup();
    void update();
    void draw();
	void drawLabels();
	void initVolume();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofEasyCam cam;
    
    ofxVolumetrics myVolume;
    unsigned char * volumeData;
    int volWidth, volHeight, volDepth;
    ofImage background;
    ofxImageSequencePlayer imageSequence;
    bool linearFilter;

	
	float clipPlaneDepth;
	float FBOq, Zq, thresh, density, dithering;
	
	ofVec3f	volPos;
	ofVec3f	volSize;
	ofVec3f	volOffset;
	ofVec3f voxelSize;

	ofVec3f uiClamp;	// same as ui but using values only inside volume
	ofPlane rayPlane;


};
