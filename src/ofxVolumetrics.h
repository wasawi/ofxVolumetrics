#pragma once

/*      ofxVolumetrics - render volumetric data on the GPU

 Written by Timothy Scaffidi (http://timothyscaffidi.com)
 Volumetric rendering algorithm adapted from Peter Trier (http://www.daimi.au.dk/~trier/?page_id=98)

*/

#include "ofFbo.h"
#include "ofShader.h"
#include "ofxTexture3d.h"
#include "ofxImageSequencePlayer.h"
#include "ofxVolume.h"

class ofxVolumetrics
{
public:
    ofxVolumetrics();
    virtual ~ofxVolumetrics();
	void setup(int w, int h, int d, ofVec3f voxelSize, bool usePowerOfTwoTexSize=false);
    void destroy();
    void updateVolumeData(unsigned char * data, int w, int h, int d, int xOffset, int yOffset, int zOffset);
    void drawVolume(float x, float y, float z, float size, int zTexOffset);
    void drawVolume(float x, float y, float z, float w, float h, float d, int zTexOffset);
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
    void setXyQuality(float q);
    void setZQuality(float q);
    void setThreshold(float t);
    void setDensity(float d);
    void setRenderSettings(float xyQuality, float zQuality, float dens, float thresh);
    void setVolumeTextureFilterMode(GLint filterMode);
	
	//mine--------------------------------------------------------------
	void setup(ofxVolume* _volume,
			   ofVec3f _voxelSize = ofVec3f(1,1,1),
			   bool usePowerOfTwoTexSize=false,
			   GLint internalformat=GL_RGBA);
	
    void setVolume(ofxVolume* _volume,
				   bool usePowerOfTwoTexSize=false,
				   GLint internalformat=GL_RGBA);
	
    void setup(unsigned char * _data,
			   ofVec3f _volSize,
			   ofVec3f _voxelSize = ofVec3f(1,1,1),
			   bool usePowerOfTwoTexSize=false,
			   GLint internalformat=GL_RGBA);
    void setVolume(unsigned char * _data,
				   ofVec3f _volSize,
				   bool usePowerOfTwoTexSize=false,
				   GLint internalformat=GL_RGBA);
	void update();
    void draw(float x, float y, float w, float h);
    void drawCube(float size);
	
	float getDithering();
	
    void setElevation(float elev);
    void setAzimuth(float azi);
	void setDithering(float d);
	void setClipDepth(float depth);
	
	ofVec3f getVolOffset();
    ofVec3f getVoxelRatio();
    ofVec3f getCubeSize();
    ofVec3f getCubePos();
	//-mine
	
protected:
private:
    void drawRGBCube();
    void updateRenderDimentions();
	void setCubeSize(ofVec3f _volumeSize, ofVec3f _voxelRatio);

	//mine--------------------------------------------------------------
	ofxVolume* vol;
	ofVec3f volOffset;
    ofVec3f cubeSize;
    ofVec3f cubePos;
	
	GLint filterMode;
	float dithering;
    float clipPlaneDepth, elevation, azimuth;
	bool bNewCode;
	//-mine
    
	
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
};
