#pragma once

//  Written by Timothy Scaffidi (http://timothyscaffidi.com)

#include "ofTexture.h"
#include "ofMain.h"

#include "ofxVoxels.h"
#include "ofxPoint.h"

class ofxTextureData3d : public ofTextureData
{
    public:
    ofxTextureData3d() {
		textureID = 0;

		glTypeInternal = GL_RGB8;
		textureTarget = GL_TEXTURE_3D;

        glType = GL_RGBA;
		pixelType = GL_UNSIGNED_BYTE;

		tex_t = 0;
		tex_u = 0;
		tex_v = 0;
		tex_w = 0;
		tex_h = 0;
		tex_d = 0;
		width = 0;
		height = 0;
		depth = 0;

		bFlipTexture = false;
		compressionType = OF_COMPRESS_NONE;
		bAllocated = false;
	}

	float tex_v;
	float tex_d;
	float depth;
    int glType, pixelType;
};

class ofxTexture3d
{
    public:
        ofxTexture3d();
        virtual ~ofxTexture3d();
		void allocate(ofxIntPoint size, int internalGlDataType);
		void allocate(ofxPoint size, int internalGlDataType);
		void allocate(ofVec3f size, int internalGlDataType);
        void allocate(int w, int h, int d, int internalGlDataType);
	
//		void loadData(ofxVolume vol, ofVec3f offset, int glFormat);
		void loadData(ofxVoxels vox);
		void loadData(unsigned char * data, ofVec3f size, ofVec3f offset, int glFormat);
        void loadData(unsigned char * data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat);
        void loadData(float* data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat);
        void loadData(unsigned short* data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat);
        void loadData(ofPixels & pix, int d, int xOffset, int yOffset, int zOffset);
        void loadData(ofShortPixels & pix, int d, int xOffset, int yOffset, int zOffset);
        void loadData(ofFloatPixels & pix, int d, int xOffset, int yOffset, int zOffset);
        void bind();
        void unbind();
        void clear();
	
		bool bAllocated();
		bool isAllocated();
		// in openGL3+ use 1 channel GL_R as luminance instead of red channel
		void setRGToRGBASwizzles(bool rToRGBSwizzles);

        ofxTextureData3d getTextureData();
		float getHeight();
		float getWidth();
		float getDepth();
    protected:
        void loadData(void * data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat);
		void enableTextureTarget();
		void disableTextureTarget();

    private:
        ofxTextureData3d texData;
	
	
};


