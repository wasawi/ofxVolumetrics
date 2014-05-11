

#include "ofxVolumetrics.h"
#include "ofMain.h"

//--------------------------------------------------------------
ofxVolumetrics::ofxVolumetrics()
{
    quality = ofVec3f(1.0);
    threshold = 1.0/255.0;
    density = 1.0;
	dithering = 0;
    volWidth = renderWidth = 0;
    volHeight = renderHeight = 0;
    volDepth = 0;
    bIsInitialized	= false;

    /* Front side */
    volNormals[0] = ofVec3f(0.0, 0.0, 1.0);
    volNormals[1] = ofVec3f(0.0, 0.0, 1.0);
    volNormals[2] = ofVec3f(0.0, 0.0, 1.0);
    volNormals[3] = ofVec3f(0.0, 0.0, 1.0);

    volVerts[0] = ofVec3f(1.0, 1.0, 1.0);
    volVerts[1] = ofVec3f(0.0, 1.0, 1.0);
    volVerts[2] = ofVec3f(0.0, 0.0, 1.0);
    volVerts[3] = ofVec3f(1.0, 0.0, 1.0);


    /* Right side */
    volNormals[4] = ofVec3f(1.0, 0.0, 0.0);
    volNormals[5] = ofVec3f(1.0, 0.0, 0.0);
    volNormals[6] = ofVec3f(1.0, 0.0, 0.0);
    volNormals[7] = ofVec3f(1.0, 0.0, 0.0);

    volVerts[4] = ofVec3f(1.0, 1.0, 1.0);
    volVerts[5] = ofVec3f(1.0, 0.0, 1.0);
    volVerts[6] = ofVec3f(1.0, 0.0, 0.0);
    volVerts[7] = ofVec3f(1.0, 1.0, 0.0);

    /* Top side */
    volNormals[8] = ofVec3f(0.0, 1.0, 0.0);
    volNormals[9] = ofVec3f(0.0, 1.0, 0.0);
    volNormals[10] = ofVec3f(0.0, 1.0, 0.0);
    volNormals[11] = ofVec3f(0.0, 1.0, 0.0);

    volVerts[8] = ofVec3f(1.0, 1.0, 1.0);
    volVerts[9] = ofVec3f(1.0, 1.0, 0.0);
    volVerts[10] = ofVec3f(0.0, 1.0, 0.0);
    volVerts[11] = ofVec3f(0.0, 1.0, 1.0);

    /* Left side */
    volNormals[12] = ofVec3f(-1.0, 0.0, 0.0);
    volNormals[13] = ofVec3f(-1.0, 0.0, 0.0);
    volNormals[14] = ofVec3f(-1.0, 0.0, 0.0);
    volNormals[15] = ofVec3f(-1.0, 0.0, 0.0);

    volVerts[12] = ofVec3f(0.0, 1.0, 1.0);
    volVerts[13] = ofVec3f(0.0, 1.0, 0.0);
    volVerts[14] = ofVec3f(0.0, 0.0, 0.0);
    volVerts[15] = ofVec3f(0.0, 0.0, 1.0);

    /* Bottom side */
    volNormals[16] = ofVec3f(0.0, -1.0, 0.0);
    volNormals[17] = ofVec3f(0.0, -1.0, 0.0);
    volNormals[18] = ofVec3f(0.0, -1.0, 0.0);
    volNormals[19] = ofVec3f(0.0, -1.0, 0.0);

    volVerts[16] = ofVec3f(0.0, 0.0, 0.0);
    volVerts[17] = ofVec3f(1.0, 0.0, 0.0);
    volVerts[18] = ofVec3f(1.0, 0.0, 1.0);
    volVerts[19] = ofVec3f(0.0, 0.0, 1.0);

    /* Back side */
    volNormals[20] = ofVec3f(0.0, 0.0, -1.0);
    volNormals[21] = ofVec3f(0.0, 0.0, -1.0);
    volNormals[22] = ofVec3f(0.0, 0.0, -1.0);
    volNormals[23] = ofVec3f(0.0, 0.0, -1.0);

    volVerts[20] = ofVec3f(1.0, 0.0, 0.0);
    volVerts[21] = ofVec3f(0.0, 0.0, 0.0);
    volVerts[22] = ofVec3f(0.0, 1.0, 0.0);
    volVerts[23] = ofVec3f(1.0, 1.0, 0.0);
}
//--------------------------------------------------------------
ofxVolumetrics::~ofxVolumetrics()
{
    destroy();
}

//--------------------------------------------------------------
void ofxVolumetrics::destroy()
{
    volumeShader.unload();
    volumeTexture.clear();
    //    fboBackground.destroy();
    //    fboRender.destroy();
}

//--------------------------------------------------------------
void ofxVolumetrics::setup(ofxVolume* _volume, ofVec3f _voxelRatio, bool usePowerOfTwoTexSize, GLint internalformat)
{
    // FIXING GUI ERROR, https://github.com/openframeworks/openFrameworks/issues/1515
    GLuint clearErrors = glGetError();
	
	// load shader in current project
	volumeShader.load("shaders/ofxVolumetrics");

	// load shader in addon
//	volumeShader.load("../../../../ofxVolumetrics/src/shaders/ofxVolumetrics");			// accepts Lumminance texture
//	volumeShader.load("../../../../ofxVolumetrics/src/shaders/ofxVolumetricsOriginal"); // accepts RGBA texture only
	
	// load volume to 3dTexture
    setVolume(_volume, usePowerOfTwoTexSize, internalformat);
	
	setCubeSize((ofVec3f)vol->getSize(), _voxelRatio);
    updateRenderDimentions();
    
	bIsInitialized = true;
}

//--------------------------------------------------------------
void ofxVolumetrics::setVolume(ofxVolume* _volume, bool usePowerOfTwoTexSize, GLint internalformat)
{
    vol = _volume;
	volWidth		= vol->getWidth();
	volHeight		= vol->getHeight();
	volDepth		= vol->getDepth();
	
    bIsPowerOfTwo = usePowerOfTwoTexSize;
    if(bIsPowerOfTwo){
		volWidthPOT		= ofNextPow2(vol->getWidth());
		volHeightPOT	= ofNextPow2(vol->getHeight());
		volDepthPOT		= ofNextPow2(vol->getDepth());
		
		ofLogVerbose() << "ofxVolumetrics::setVolume(): Using power of two texture size. Requested: "
		<< volWidth		<<"x"<<	volHeight	<<"x"<<	volDepth <<". Actual: "
		<< volWidthPOT	<<"x"<<	volHeightPOT<<"x"<< volDepthPOT <<".";
		
/*		// if using cropped power of two, blank out the extra memory
		volumeTexture.allocate(volWidthPOT, volHeightPOT, volDepthPOT, GL_RGBA);
        unsigned char * data;
        data = new unsigned char[volWidthPOT*volHeightPOT*volDepthPOT*4];
        memset(data,0,volWidthPOT*volHeightPOT*volDepthPOT*4);
        volumeTexture.loadData(vol->getVoxels(),volWidthPOT, volHeightPOT, volDepthPOT, 0,0,0,GL_RGBA);
*/
		
//		unsigned char * data;
//		data = new unsigned char[(int)volPot.getVolume()*4];
//		memset(data,0,(int)volPot.getVolume()*4);
//		volumeTexture.allocate(volPot.getSize(), internalformat);
//		volumeTexture.loadData(data,volPot.getWidth(), volPot.getHeight(), volPot.getDepth(), 0,0,0,internalformat);
//		volumeTexture.loadData(data, volPot.size, volOffset, internalformat);
//		volumeTexture.loadData(data,volWidthPOT, volHeightPOT, volDepthPOT, 0,0,0,GL_RGBA);
		
		volumeTexture.allocate(volWidthPOT, volHeightPOT, volDepthPOT, internalformat);
		volumeTexture.loadData(*vol, volOffset, internalformat);
		setVolumeTextureFilterMode(filterMode);
    }else{
		volWidthPOT		= volWidth;
		volHeightPOT	= volHeight;
		volDepthPOT		= volDepth;

		ofLogVerbose() << "ofxVolumetrics::setup(): Requested: "
		<< vol->getWidth() << "x" <<vol->getHeight()<<"x"<<vol->getDepth();

        volumeTexture.allocate(vol->getSize(), internalformat);
        volumeTexture.loadData(*vol, volOffset, internalformat);
		setVolumeTextureFilterMode(filterMode);
    }
}

//--------------------------------------------------------------
void ofxVolumetrics::updateRenderDimentions()
{

    if((int)(ofGetWidth() * quality.x) != renderWidth)
    {
        renderWidth = ofGetWidth()*quality.x;
        renderHeight = ofGetHeight()*quality.x;
        fboRender.allocate(renderWidth, renderHeight, GL_RGBA);
    }
	ofLogVerbose("ofxVolumetrics::updateRenderDimentions()")
	<< " fbo dimensions = "<< renderWidth<< "x" <<renderHeight;
}

//--------------------------------------------------------------
void ofxVolumetrics::update()
{
/*    updateRenderDimentions();
	cout <<"MY UPDATE*************************\n";
	cout <<"cubeSize		" << cubeSize<<".\n";
	cout <<"render		" << renderWidth<<"x"<< renderHeight<<".\n";
	cout <<"vol dim		" << volWidth<<"x"<<volHeight<<"x"<<volDepth<<".\n";
	cout <<"volPot dim	" << volWidthPOT<<"x"<<volHeightPOT<<"x"<<volDepthPOT<<".\n";
*/
	ofMatrix4x4 modelView;
    ofMatrix4x4 projection;
    GLfloat modl[16], proj[16];
    GLint color[4];
    ofVec3f scale,t;
    ofQuaternion a,b;
	
	// get the view from Opengl matrix
	glGetFloatv( GL_MODELVIEW_MATRIX, modl);
	glGetFloatv(GL_PROJECTION_MATRIX, proj);
	glGetIntegerv(GL_CURRENT_COLOR, color);
	ofMatrix4x4(modl).decompose(t, a, scale, b);
	
    GLint cull_mode;
    glGetIntegerv(GL_FRONT_FACE, &cull_mode);
    GLint cull_mode_fbo = (scale.x * scale.y * scale.z) > 0 ? GL_CCW : GL_CW;
	
    // This allows us to draw the slices around the fbo texture
    ofEnableDepthTest();
    // raycasting pass
    fboRender.begin();
    volumeShader.begin();
    ofClear(0,0,0,0);
	
	//load matricies from outside the FBO
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(proj);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modl);
	
    ofPushView();
//    glScalef (-1.0, 1.0, 1.0);  // draw the volume with correct map
    ofTranslate(cubePos.x - cubeSize.x/2, cubePos.y - cubeSize.y/2, cubePos.z - cubeSize.z/2);
    ofScale(cubeSize.x,cubeSize.y,cubeSize.z);
	
	
    //pass parameters to the shader
    glActiveTexture(GL_TEXTURE1);
    volumeTexture.bind();
    volumeShader.setUniform1i("volume_tex", 1); // volume texture reference
    volumeTexture.unbind();
    glActiveTexture(GL_TEXTURE0);
	
	volumeShader.setUniform3f("vol_d", (float)volWidth, (float)volHeight, (float)volDepth);
	volumeShader.setUniform3f("vol_d_pot", (float)volWidthPOT, (float)volHeightPOT, (float)volDepthPOT);
    // dimensions of the background texture
    volumeShader.setUniform2f("bg_d", (float)renderWidth, (float)renderHeight);
    // used for animation so that we dont have to upload the entire volume every time
	
    volumeShader.setUniform1f("quality", quality.z); // 0 ... 1
    volumeShader.setUniform1f("density", density); // 0 ... 1
    volumeShader.setUniform1f("dithering", dithering); // 0 ... 1
    volumeShader.setUniform1f("threshold", threshold);//(float)mouseX/(float)ofGetWidth());
	volumeShader.setUniform1f("zoffset",0.0f); // used for animation so that we dont have to upload the entire volume every time

	// cutting plane
    //    volumeShader.setUniform1f("zoffset", zTexOffset);
	volumeShader.setUniform1f("clipPlaneDepth", clipPlaneDepth);//-sizeFactor*planeCoords->y);
    volumeShader.setUniform1f("azimuth", 360*azimuth);
    volumeShader.setUniform1f("elevation", 360*elevation);
	
    glFrontFace(cull_mode_fbo);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    drawRGBCube();
    glDisable(GL_CULL_FACE);
    glFrontFace(cull_mode);
    volumeShader.end();
	
    ofPopView();
	
    drawCube(1.);
//    ofDisableAntiAliasing();
//    ofDisableSmoothing();
	//  drawSlices(1.011);
	//  drawLimits(1.011);
    ofDisableDepthTest();
	
	//  drawAxis(.01);
	//  drawSphere();
    fboRender.end();
	
	glColor4iv(color);
    ofSetupScreenOrtho();//ofGetWidth(), ofGetHeight(),OF_ORIENTATION_DEFAULT,false,0,1000);
	fboRender.draw(0,ofGetHeight(),ofGetWidth(),-ofGetHeight());
}

//--------------------------------------------------------------
void ofxVolumetrics::draw(float x, float y, float w, float h){
    ofPushView();
	ofSetupScreenOrtho();//ofGetWidth(), ofGetHeight(),OF_ORIENTATION_DEFAULT,false,0,1000);
	fboRender.draw(x, y, w, h);
    ofPopView();
	
	//  Draw the color mapping used on the screen, for viewer reference
	//  lutTexture.draw(300, 50, 0, 256, 20);
}

//--------------------------------------------------------------
void ofxVolumetrics::drawRGBCube()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(ofVec3f), volVerts);
    glNormalPointer(GL_FLOAT, sizeof(ofVec3f), volNormals);
    glColorPointer(3,GL_FLOAT, sizeof(ofVec3f), volVerts);
    glTexCoordPointer(3, GL_FLOAT, sizeof(ofVec3f), volVerts);

    glDrawArrays(GL_QUADS, 0, 24);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

//************ getters ***************//

bool ofxVolumetrics::isInitialized()
{
    return bIsInitialized;
}
int ofxVolumetrics::getVolumeWidth()
{
	return vol->getWidth();
}
int ofxVolumetrics::getVolumeHeight()
{
	return vol->getHeight();
}
int ofxVolumetrics::getVolumeDepth()
{
	return vol->getHeight();
}
int ofxVolumetrics::getRenderWidth()
{
    return renderWidth;
}
int ofxVolumetrics::getRenderHeight()
{
    return renderHeight;
}
float ofxVolumetrics::getXyQuality()
{
    return quality.x;
}
float ofxVolumetrics::getZQuality()
{
    return quality.z;
}
float ofxVolumetrics::getThreshold()
{
    return threshold;
}
float ofxVolumetrics::getDensity()
{
    return density;
}
ofFbo & ofxVolumetrics::getFboReference(){
    return fboRender;
}
float ofxVolumetrics::getDithering()
{
    return dithering;
}
ofVec3f ofxVolumetrics::getVolOffset(){
    return volOffset;
}
ofVec3f ofxVolumetrics::getVoxelRatio(){
    return voxelRatio;
}
ofVec3f ofxVolumetrics::getCubeSize(){
    return cubeSize;
}
ofVec3f ofxVolumetrics::getCubePos(){
    return cubePos;
}

//************ setters ***************//


void ofxVolumetrics::setXyQuality(float q)
{
    quality.x = MAX(q,0.01);
}
void ofxVolumetrics::setZQuality(float q)
{
    quality.z = MAX(q,0.01);
}
void ofxVolumetrics::setThreshold(float t)
{
    threshold = ofClamp(t,0.0,1.0);
}
void ofxVolumetrics::setDensity(float d)
{
    density = MAX(d,0.0);
}
void ofxVolumetrics::setElevation(float elev){
    elevation = elev;
}
void ofxVolumetrics::setAzimuth(float azi){
    azimuth = azi;
}
void ofxVolumetrics::setDithering(float d)
{
    dithering = ofClamp(d,0.0,1.0);
}
void ofxVolumetrics::setClipDepth(float depth){
    clipPlaneDepth = ofClamp(depth,-1.0,1.0);;
}
void ofxVolumetrics::setRenderSettings(float xyQuality, float zQuality, float dens, float thresh)
{
    setXyQuality(xyQuality);
    setZQuality(zQuality);
    setDensity(dens);
    setThreshold(thresh);
}
void ofxVolumetrics::setVolumeTextureFilterMode(GLint _filterMode) {
    if(_filterMode != GL_NEAREST && _filterMode != GL_LINEAR) return;
	
	filterMode=_filterMode;
	
    volumeTexture.bind();
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, _filterMode);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, _filterMode);
    volumeTexture.unbind();
}
void ofxVolumetrics::setCubeSize(ofVec3f _volumeSize, ofVec3f _voxelRatio) {
	float screensize  = ofGetHeight();
    ofVec3f volumeSize = _voxelRatio * _volumeSize;
    float maxDim = max(max(volumeSize.x, volumeSize.y), volumeSize.z);
    cubeSize = volumeSize * screensize / maxDim;
	ofLogNotice("ofxVolumetrics::setCubeSize()") << cubeSize;

}
//--------------------------------------------------------------

//--------------------------------------------------------------
void ofxVolumetrics::drawCube(float size)
{
    float f = 1.001;
	
    // Draw Cube
    ofPushStyle();
	ofSetColor(120);
	ofNoFill();
    ofPushMatrix();
	ofScale(cubeSize.x*f, cubeSize.y*f, cubeSize.z*f);
	ofDrawBox(0,0,0,size);
    ofPopMatrix();
    ofPopStyle();
}

