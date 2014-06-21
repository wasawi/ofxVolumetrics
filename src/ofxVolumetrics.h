#pragma once

/*      ofxVolumetrics - render volumetric data on the GPU

 Written by Timothy Scaffidi (http://timothyscaffidi.com)
 Volumetric rendering algorithm adapted from Peter Trier (http://www.daimi.au.dk/~trier/?page_id=98)

*/

#include "ofFbo.h"
#include "ofShader.h"
#include "ofxTexture3d.h"
#include "ofxImageSequencePlayer.h"

class ofxVolumetrics
{
public:
    ofxVolumetrics();
    virtual ~ofxVolumetrics();
    void setup(int w, int h, int d, ofVec3f voxelSize);
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
	float getDithering();
    void setXyQuality(float q);
    void setZQuality(float q);
    void setThreshold(float t);
    void setDensity(float d);
	void setDithering(float d);
    void setRenderSettings(float xyQuality, float zQuality, float dens, float thresh);
    void setVolumeTextureFilterMode(GLint filterMode);

	void setElevation(float elev);
	void setAzimuth(float azi);
	void setClipDepth(float depth);

	float getElevation();
	float getAzimuth();
	float getClipDepth();

	void createCLUT();
	ofTexture lutTexture;
	unsigned char * colorPalette;
	void setRGBA_mean(float RGBA[]);
	void setRGBA_var(float RBBA[]);
	void setRGBA(float RGBA_val[]);
	float RGBA_mean[4], RGBA_var[4], RGBA[4];



protected:
private:
    void drawRGBCube();
    void updateRenderDimentions();

    ofFbo fboRender;
    ofShader volumeShader;
    ofxTexture3d volumeTexture;
    //ofMesh volumeMesh; //unfortunately this only supports 2d texture coordinates at the moment.
    ofVec3f volVerts[24];
    ofVec3f volNormals[24];
    ofVec3f voxelRatio;
    bool bIsInitialized;
    int volWidth, volHeight, volDepth;
    int renderWidth, renderHeight;
	
    ofVec3f quality;
    float threshold;
    float density;
	float dithering;
	float clipPlaneDepth;
	float elevation;
	float azimuth;
};
