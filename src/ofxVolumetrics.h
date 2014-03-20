#pragma once

/*      ofxVolumetrics - render volumetric data on the GPU

 Written by Timothy Scaffidi (http://timothyscaffidi.com)
 Volumetric rendering algorithm adapted from Peter Trier (http://www.daimi.au.dk/~trier/?page_id=98)

*/

#include "ofFbo.h"
#include "ofShader.h"
#include "ofxTexture3d.h"
#include "ofxImageSequencePlayer.h"
#include "ofxRay.h"

class ofxVolumetrics{
public:
	
    ofxVolumetrics();
    virtual ~ofxVolumetrics();
    void setup(int w, int h, int d, ofVec3f voxelSize, bool usePowerOfTwoTexSize);
    void destroy();
    void updateVolumeData(unsigned char * data, int w, int h, int d, int xOffset, int yOffset, int zOffset);
    void update(float x, float y, float z, float size, int zTexOffset);
    void drawVolume(float x, float y, float z, float w, float h, float d, int zTexOffset);
	void updateVolume(ofVec3f& volPos, ofVec3f& volSize, int zTexOffset);
	void draw(float x, float y, float w, float h);
	
	// getters
    bool isInitialized();
    int getVolumeWidth();
    int getVolumeHeight();
    int getVolumeDepth();
    ofFbo & getFboReference();
    int getRenderWidth();
    int getRenderHeight();
    float getXyQuality();
    float getZQuality();
    float getThreshold();
    float getDensity();
	float getDithering();

	// setters
    void setXyQuality(float q);
    void setZQuality(float q);
    void setThreshold(float t);
    void setDensity(float d);
    void setRenderSettings(float xyQuality, float zQuality, float dens, float thresh);
    void setVolumeTextureFilterMode(GLint filterMode);
	void setDithering(float d);
	void setElevation(float elev);
	void setAzimuth(float azi);
	void setClipDepth(float depth);
	void setCoronalPlane(float f);
	void setSagittalPlane(float f);
	void setAxialPlane(float f);
	
	void setSlices(ofVec3f* _planes);
	void setRayPlane(ofPlane* _rayPlane);
	bool getIntersection(ofCamera* cam,ofVec3f &intersectionPosition);
	
protected:
private:
    void drawRGBCube();
    void updateRenderDimentions();
	void drawSlices(float size);
	void drawSphere(float size);
	void drawCube(float size);
	void drawAxis(float size);
	void drawLimits(float size);
	void drawBox(const ofPoint& position, const ofPoint& size);
	
    ofFbo fboRender;
    ofShader volumeShader;
    ofxTexture3d volumeTexture;
    //ofMesh volumeMesh; //unfortunately this only supports 2d texture coordinates at the moment.
    ofVec3f volVerts[24];
    ofVec3f volNormals[24];
    ofVec3f voxelRatio;
    bool bIsInitialized;
    int volWidth, volHeight, volDepth;
    int volWidthPOT, volHeightPOT, volDepthPOT;
    bool bIsPowerOfTwo;

    ofVec3f quality;
    float threshold;
    float density;
    int renderWidth, renderHeight;
	
	float dithering;
	float clipPlaneDepth;
	float elevation;
	float azimuth;
	
	float sizeFactor;
	
	ofVec3f cubeSize;
	ofVec3f cubePos;

	//ofRay objects
	ofVec3f *planeCoords;
	ofRectangle plane;
	
	// debug
	bool bCameraView;
	
	//ofRay objects
	ofPlane		*rayPlane;
	ofRay		mouseRay;
};


