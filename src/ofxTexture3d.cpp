#include "ofxTexture3d.h"
#include "ofTexture.cpp"

//----------------------------------------------------------
ofxTexture3d::ofxTexture3d()
{
    //ctor
}
//----------------------------------------------------------
ofxTexture3d::~ofxTexture3d()
{
    release(texData.textureID);
}
//----------------------------------------------------------
void ofxTexture3d::allocate(ofxIntPoint size, int internalGlDataType)
{
	allocate(size.x, size.y, size.z, internalGlDataType);
}
//----------------------------------------------------------
void ofxTexture3d::allocate(ofxPoint size, int internalGlDataType)
{
	allocate((int)size.x, (int)size.y, (int)size.z, internalGlDataType);
}
//----------------------------------------------------------
void ofxTexture3d::allocate(ofVec3f size, int internalGlDataType)
{
	allocate(size.x, size.y, size.z, internalGlDataType);
}
//----------------------------------------------------------
void ofxTexture3d::allocate(int w, int h, int d, int internalGlDataType)
{
    texData.tex_w = w;
    texData.tex_h = h;
    texData.tex_d = d;
    texData.tex_t = w;
    texData.tex_u = h;
    texData.tex_v = d;
    texData.textureTarget = GL_TEXTURE_3D;

    texData.glTypeInternal = internalGlDataType;
    // get the glType (format) and pixelType (type) corresponding to the glTypeInteral (internalFormat)
    texData.glType = ofGetGLFormatFromInternal(texData.glTypeInternal);
    texData.pixelType = ofGetGlTypeFromInternal(texData.glTypeInternal);
    // attempt to free the previous bound texture, if we can:
    clear();

    glGenTextures(1, (GLuint *)&texData.textureID);
    retain(texData.textureID);
    glEnable(texData.textureTarget);
    glBindTexture(texData.textureTarget, (GLuint)texData.textureID);

    glTexImage3D(texData.textureTarget, 0, texData.glTypeInternal, (GLint)texData.tex_w, (GLint)texData.tex_h, (GLint)texData.tex_d, 0, texData.glType, texData.pixelType, 0);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glDisable(texData.textureTarget);

    texData.width = w;
    texData.height = h;
    texData.depth = d;
    texData.bFlipTexture = false;
    texData.bAllocated = true;
}
//----------------------------------------------------------
void ofxTexture3d::loadData(ofxVolume& vol, ofVec3f offset, int glFormat)
{
    loadData(vol.getVoxels(), vol.getWidth(), vol.getHeight(), vol.getDepth(), offset.x, offset.y, offset.z, glFormat);
}
//----------------------------------------------------------
void ofxTexture3d::loadData(ofxVoxels vox)
{
    loadData(vox.getVoxels(), vox.getWidth(), vox.getHeight(), vox.getDepth(), vox.getOffset().x, vox.getOffset().y, vox.getOffset().z, vox.getGlFormat());
}
//----------------------------------------------------------
void ofxTexture3d::loadData(unsigned char * data, ofVec3f size, ofVec3f offset, int glFormat)
{
    loadData((void *)data, size.x, size.y, size.z, offset.x, offset.y, offset.z, glFormat);
}
//----------------------------------------------------------
void ofxTexture3d::loadData(unsigned char * data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat)
{
    loadData((void *)data, w, h, d, xOffset, yOffset, zOffset, glFormat);
}
//----------------------------------------------------------
void ofxTexture3d::loadData(float* data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat)
{
    loadData((void *)data, w, h, d, xOffset, yOffset, zOffset, glFormat);
}
//----------------------------------------------------------
void ofxTexture3d::loadData(unsigned short* data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat)
{
    loadData((void *)data, w, h, d, xOffset, yOffset, zOffset, glFormat);
}
//----------------------------------------------------------
void ofxTexture3d::loadData(ofPixels & pix, int d, int xOffset, int yOffset, int zOffset)
{
    loadData(pix.getPixels(), pix.getWidth(), pix.getHeight(), d, xOffset, yOffset, zOffset, ofGetGlFormat(pix));
}
//----------------------------------------------------------
void ofxTexture3d::loadData(ofShortPixels & pix, int d, int xOffset, int yOffset, int zOffset)
{
    loadData(pix.getPixels(), pix.getWidth(), pix.getHeight(), d, xOffset, yOffset, zOffset, ofGetGlFormat(pix));
}
//----------------------------------------------------------
void ofxTexture3d::loadData(ofFloatPixels & pix, int d, int xOffset, int yOffset, int zOffset)
{
    loadData(pix.getPixels(), pix.getWidth(), pix.getHeight(), d, xOffset, yOffset, zOffset, ofGetGlFormat(pix));
}
//----------------------------------------------------------
void ofxTexture3d::loadData(void * data, int w, int h, int d, int xOffset, int yOffset, int zOffset, int glFormat)
{
    if(glFormat!=texData.glType)
    {
        ofLogError() << "ofxTexture3d::loadData() failed to upload format " <<  ofGetGlInternalFormatName(glFormat) << " data to " << ofGetGlInternalFormatName(texData.glType) << " texture" <<endl;
        return;
    }

    if(w > texData.tex_w || h > texData.tex_h || d > texData.tex_d)
    {
        ofLogError() << "ofxTexture3d::loadData() failed to upload " <<  w << "x" << h << "x" << d << " data to " << texData.tex_w << "x" << texData.tex_h << "x" << texData.tex_d << " texture";
        return;
    }

	// bugFix: https://github.com/timscaffidi/ofxVolumetrics/issues/5#issuecomment-42230018
	ofSetPixelStorei(w, 1, ofGetNumChannelsFromGLFormat(glFormat));
	
    glEnable(texData.textureTarget);
    glBindTexture(texData.textureTarget, (GLuint) texData.textureID);
    glTexSubImage3D(texData.textureTarget, 0, xOffset, yOffset, zOffset, w, h, d, texData.glType, texData.pixelType, data);
    glDisable(texData.textureTarget);

}
//----------------------------------------------------------
void ofxTexture3d::clear()
{
    release(texData.textureID);
    texData.textureID  = 0;
    texData.bAllocated = false;
}
//----------------------------------------------------------
void ofxTexture3d::bind()
{
    //we could check if it has been allocated - but we don't do that in draw()
    glEnable(texData.textureTarget);
    glBindTexture( texData.textureTarget, (GLuint)texData.textureID);
}
//----------------------------------------------------------
void ofxTexture3d::unbind()
{
    glDisable(texData.textureTarget);
}
//----------------------------------------------------------
bool ofxTexture3d::bAllocated(){
	return texData.bAllocated;
}
//----------------------------------------------------------
bool ofxTexture3d::isAllocated(){
	return texData.bAllocated;
}
//----------------------------------------------------------
void ofxTexture3d::setRGToRGBASwizzles(bool rToRGBSwizzles){
#ifndef TARGET_OPENGLES
	enableTextureTarget();
	
	glBindTexture(texData.textureTarget, (GLuint)texData.textureID);
	if(rToRGBSwizzles){
		if(texData.glTypeInternal==GL_R8 ||
		   texData.glTypeInternal==GL_R16 ||
		   texData.glTypeInternal==GL_R32F){
			glTexParameteri(texData.textureTarget, GL_TEXTURE_SWIZZLE_R, GL_RED);
			glTexParameteri(texData.textureTarget, GL_TEXTURE_SWIZZLE_G, GL_RED);
			glTexParameteri(texData.textureTarget, GL_TEXTURE_SWIZZLE_B, GL_RED);
			
		}else if(texData.glTypeInternal==GL_RG8 ||
				 texData.glTypeInternal==GL_RG16 ||
				 texData.glTypeInternal==GL_RG32F){
			glTexParameteri(texData.textureTarget, GL_TEXTURE_SWIZZLE_R, GL_RED);
			glTexParameteri(texData.textureTarget, GL_TEXTURE_SWIZZLE_G, GL_RED);
			glTexParameteri(texData.textureTarget, GL_TEXTURE_SWIZZLE_B, GL_RED);
			glTexParameteri(texData.textureTarget, GL_TEXTURE_SWIZZLE_A, GL_GREEN);
		}
	}else{
		if(texData.glTypeInternal==GL_R8 ||
		   texData.glTypeInternal==GL_R16 ||
		   texData.glTypeInternal==GL_R32F){
			glTexParameteri(texData.textureTarget, GL_TEXTURE_SWIZZLE_R, GL_RED);
			glTexParameteri(texData.textureTarget, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
			glTexParameteri(texData.textureTarget, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
			
		}else if(texData.glTypeInternal==GL_RG8 ||
				 texData.glTypeInternal==GL_RG16 ||
				 texData.glTypeInternal==GL_RG32F){
			glTexParameteri(texData.textureTarget, GL_TEXTURE_SWIZZLE_R, GL_RED);
			glTexParameteri(texData.textureTarget, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
			glTexParameteri(texData.textureTarget, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
			glTexParameteri(texData.textureTarget, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);
		}
	}
	
	glBindTexture( texData.textureTarget, 0);
	disableTextureTarget();
#endif
}
//----------------------------------------------------------
void ofxTexture3d::enableTextureTarget(){
	if(ofGetGLRenderer()) ofGetGLRenderer()->enableTextureTarget(texData.textureTarget);
}
//----------------------------------------------------------
void ofxTexture3d::disableTextureTarget(){
	if(ofGetGLRenderer()) ofGetGLRenderer()->disableTextureTarget(texData.textureTarget);
}
//----------------------------------------------------------
float ofxTexture3d::getHeight(){
	return texData.height;
}
//----------------------------------------------------------
float ofxTexture3d::getWidth(){
	return texData.width;
}
//----------------------------------------------------------
float ofxTexture3d::getDepth(){
	return texData.depth;
}
//----------------------------------------------------------
ofxTextureData3d ofxTexture3d::getTextureData()
{
    if(!texData.bAllocated)
    {
        ofLog(OF_LOG_ERROR, "getTextureData() - texture has not been allocated");
    }

    return texData;
}
