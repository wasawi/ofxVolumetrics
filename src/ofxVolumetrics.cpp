#include "ofxVolumetrics.h"
#include "ofMain.h"
#define STRINGIFY( A) #A

ofxVolumetrics::ofxVolumetrics()
{
    quality = ofVec3f(1.0);
    threshold = 1.0/255.0;
    density = 1.0;
    volWidth = renderWidth = 0;
    volHeight = renderHeight = 0;
    volDepth = 0;
    bIsInitialized = false;
	sizeFactor= .5;
	
	cubeSize		= ofVec3f(1, 1, 1);
	cubePos			= ofVec3f(0, 0, 0);
	bCameraView		= false;
	
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

ofxVolumetrics::~ofxVolumetrics()
{
    destroy();
}

void ofxVolumetrics::setup(int w, int h, int d, ofVec3f voxelSize, bool usePowerOfTwoTexSize, ofCamera& _cam)
{
	GLuint clearErrors = glGetError(); // FIXING GUI ERROR, https://github.com/openframeworks/openFrameworks/issues/1515
    volumeShader.load("shaders/ofxVolumetrics");
    volWidth = renderWidth = w;
    volHeight = renderHeight = h;
    volDepth = d;
    fboRender.allocate(w, h, GL_RGBA);
    volumeTexture.allocate(w, h, d, GL_LUMINANCE);
    voxelRatio = voxelSize;
	cam	= &_cam;
	
    bIsInitialized = true;
}

void ofxVolumetrics::destroy()
{
    volumeShader.unload();
//    fboBackground.destroy();
//    fboRender.destroy();
    volumeTexture.clear();
}

void ofxVolumetrics::updateVolumeData(unsigned char * data, int w, int h, int d, int xOffset, int yOffset, int zOffset)
{
    volumeTexture.loadData(data, w, h, d, xOffset, yOffset, zOffset, GL_LUMINANCE);
}

/*
void ofxVolumetrics::update(float x, float y, float z, float size, int zTexOffset)
{
    ofVec3f volumeSize = voxelRatio * ofVec3f(volWidth,volHeight,volDepth);
    float maxDim = max(max(volumeSize.x, volumeSize.y), volumeSize.z);
    volumeSize = volumeSize * size / maxDim;

    drawVolume(x, y, z, volumeSize.x, volumeSize.y, volumeSize.z, zTexOffset);
}
*/

void ofxVolumetrics::updateVolume(ofVec3f& volPos, ofVec3f& volSize, int zTexOffset)
{
//	updateRenderDimentions();
	cubeSize = volSize;
	ofMatrix4x4 modelView;
	ofMatrix4x4 projection;
	GLfloat modl[16], proj[16];
	GLint color[4];
	ofVec3f scale,t;
	ofQuaternion a,b;
	
	if (bCameraView){
		
		// get the view from the current camera
		modelView = cam->getModelViewMatrix();
		projection= cam->getProjectionMatrix();
		glGetIntegerv(GL_CURRENT_COLOR, color);
		modelView.decompose(t, a, scale, b);
		cout << ".";
	}else{
		
		//*	// get the view from Opengl matrix
		glGetFloatv( GL_MODELVIEW_MATRIX, modl);
		glGetFloatv(GL_PROJECTION_MATRIX, proj);
		glGetIntegerv(GL_CURRENT_COLOR, color);
		ofMatrix4x4(modl).decompose(t, a, scale, b);
	}
	
	GLint cull_mode;
	glGetIntegerv(GL_FRONT_FACE, &cull_mode);
	GLint cull_mode_fbo = (scale.x * scale.y * scale.z) > 0 ? GL_CCW : GL_CW;

	// This allows us to draw the slices around the fbo texture
	ofEnableDepthTest();
	/* raycasting pass */
    fboRender.begin();
    volumeShader.begin();
    ofClear(0);

	if (bCameraView){
		
		ofSetMatrixMode(OF_MATRIX_PROJECTION);
		ofLoadMatrix( projection );
		ofSetMatrixMode(OF_MATRIX_MODELVIEW);
		ofLoadMatrix( modelView );
		
	}else{
		
		//load matricies from outside the FBO
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(proj);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(modl);
	}
		
	ofPushView();
    glScalef (-1.0, 1.0, 1.0);	// draw the volume with correct map
	ofTranslate(volPos.x - cubeSize.x/2, volPos.y - cubeSize.y/2, volPos.z - cubeSize.z/2);
    ofScale(cubeSize.x,cubeSize.y,cubeSize.z);
	
	
    //pass parameters to the shader
    glActiveTexture(GL_TEXTURE1);
    volumeTexture.bind();
    volumeShader.setUniform1i("volume_tex", 1); // volume texture reference
    volumeTexture.unbind();
	glActiveTexture(GL_TEXTURE0);
	
	//dimensions of the volume texture
    volumeShader.setUniform3f("vol_d", (float)volWidth, (float)volHeight, (float)volDepth);
	// dimensions of the background texture
    volumeShader.setUniform2f("bg_d", (float)renderWidth, (float)renderHeight);
	// used for animation so that we dont have to upload the entire volume every time
    volumeShader.setUniform1f("zoffset", zTexOffset);
    volumeShader.setUniform1f("quality", quality.z); // 0 ... 1
    volumeShader.setUniform1f("density", density); // 0 ... 1
	volumeShader.setUniform1f("dithering", dithering); // 0 ... 1
    volumeShader.setUniform1f("threshold", threshold);//(float)mouseX/(float)ofGetWidth());
	volumeShader.setUniform1f("clipPlaneDepth", -sizeFactor*planeCoords->y);
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

//	drawCube(1.);
	ofDisableAntiAliasing();
	ofDisableSmoothing();
//	drawSlices(1.011);
	drawLimits(1.011);
	ofDisableDepthTest();
	
	drawAxis(.01);
//	drawSphere();
	fboRender.end();

	drawRayPlane();

}

void ofxVolumetrics::draw(float x, float y, float w, float h){
	ofPushView();
		ofSetupScreenOrtho();//ofGetWidth(), ofGetHeight(),OF_ORIENTATION_DEFAULT,false,0,1000);
		fboRender.draw(x,y, w, h);
	ofPopView();
	
//	Draw the color mapping used on the screen, for viewer reference
//	lutTexture.draw(300, 50, 0, 256, 20);
}

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
//--------------------------------------------------------------
void ofxVolumetrics::drawRayPlane()
{
	rayPlane->setCenter(ofVec3f(0,0,planeCoords->y)*cubeSize*-.5);
	rayPlane->setScale(cubeSize*.5);
	rayPlane->draw();
	
	//--------------------------------------------------------------
	ofVec3f screenMouse = ofVec3f(ofGetMouseX(), ofGetMouseY(),0);
	ofVec3f worldMouse = cam->screenToWorld(ofVec3f(screenMouse.x, screenMouse.y, 0.0f));
	ofVec3f worldMouseEnd = cam->screenToWorld(ofVec3f(screenMouse.x, screenMouse.y, 1.0f));
	ofVec3f worldMouseTransmissionVector = worldMouseEnd - worldMouse;
	mouseRay.s = worldMouse;
	mouseRay.t = worldMouseTransmissionVector;
	
	// check for intersection
	ofVec3f		intersectionPosition;
	bool doesIntersect = rayPlane->intersect(mouseRay, intersectionPosition);
	
	
	
	string label;
	label = doesIntersect ? "hits" : "misses";
	label += + " at world position " + ofToString(intersectionPosition);
	cout << label<< endl;
}

//--------------------------------------------------------------
void ofxVolumetrics::drawCube(float size)
{
	float f = .9999;
	
	// Draw Cube
	ofPushStyle();
		ofSetColor(120);
		ofNoFill();
	ofPushMatrix();
		ofScale(cubeSize.x*f,cubeSize.y*f,cubeSize.z*f);
		ofDrawBox(0,0,0,size);
	ofPopMatrix();
	ofPopStyle();
}
//--------------------------------------------------------------
void ofxVolumetrics::drawSlices(float size)
{
	float f = 1.011;
	//	cout<< cubeSize.x<< endl;
	//	cout<< axialPlane<< endl;

	ofVec3f coronalPlane	= ofVec3f(0,0,planeCoords->z);
	ofVec3f sagittalPlane	= ofVec3f(0,0,planeCoords->x);
	ofVec3f axialPlane		= ofVec3f(0,0,planeCoords->y);

	coronalPlane		*= sizeFactor;
	sagittalPlane		*= sizeFactor;
	axialPlane			*= sizeFactor;

	// Draw Slices
	ofPushStyle();
		ofSetColor(150);
		ofNoFill();
		ofSetPlaneResolution(2, 2);
	ofPushMatrix();
		ofScale(cubeSize.x*f,cubeSize.y*f,cubeSize.z*f);

		ofPushMatrix();
			ofRotateY(90);
			ofDrawPlane(sagittalPlane, size, size);
		ofPopMatrix();
		
		ofScale(-1.,-1.,-1.);
		ofDrawPlane(axialPlane, size, size);
	
		ofRotateX(90);
		ofDrawPlane(coronalPlane, size, size);
	
	ofPopMatrix();
	ofPopStyle();
}
//--------------------------------------------------------------
void ofxVolumetrics::drawLimits(float _size) {

	ofVec3f pos= *planeCoords*sizeFactor;
	float f = 1.001;
	ofVec3f size = ofVec3f(_size, _size, 0);

	ofPushStyle();
	ofSetLineWidth(1);
	ofSetColor(0,255,255);
	ofNoFill();
	
	ofPushMatrix();
		ofScale(cubeSize.x*f,cubeSize.y*f,cubeSize.z*f);
	ofVec3f coronalPlane	= ofVec3f(0,0,planeCoords->z);
	ofVec3f sagittalPlane	= ofVec3f(0,0,planeCoords->x);
	ofVec3f axialPlane		= ofVec3f(0,0,planeCoords->y);
	
	coronalPlane		*= sizeFactor;
	sagittalPlane		*= sizeFactor;
	axialPlane			*= sizeFactor;

	ofPushMatrix();
		ofRotateY(90);
		ofScale(-1.,-1.,-1.);
	
		drawBox(sagittalPlane, size);
	ofPopMatrix();
		
		ofScale(-1.,-1.,-1.);
		drawBox(axialPlane, size);
	
		ofRotateX(90);
		drawBox(coronalPlane, size);
	
	ofPopMatrix();
	ofPopStyle();
}
//--------------------------------------------------------------
void ofxVolumetrics::drawBox(const ofPoint& position, const ofPoint& size) {
	ofDrawBox( position.x, position.y, position.z, size.x, size.y, size.z);
}
//--------------------------------------------------------------
void ofxVolumetrics::drawAxis(float size) {
	ofPushStyle();
	ofPushMatrix();

	ofSetLineWidth(1);
	ofSetColor(0,255,255);
	
	ofVec3f pos= *planeCoords*sizeFactor;
	
	ofScale(cubeSize.x,cubeSize.y,cubeSize.z);
	ofRotateY(90);
	ofScale(1.,1.,-1.);

	// draw x axis
//	ofLine(pos.y - size, pos.z, pos.x, pos.y + size, pos.z, pos.x);
	// draw y axis
	ofLine(pos.y, pos.z - size, pos.x, pos.y, pos.z + size, pos.x);
	// draw z axis
	ofLine(pos.y, pos.z, pos.x - size, pos.y, pos.z, pos.x + size);

	ofPopMatrix();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofxVolumetrics::updateRenderDimentions()
{
	ofLog(OF_LOG_VERBOSE, "update render");
    if((int)(ofGetWidth() * quality.x) != renderWidth)
    {
        renderWidth = ofGetWidth()*quality.x;
        renderHeight = ofGetHeight()*quality.x;
        fboRender.allocate(renderWidth, renderHeight, GL_RGBA);
    }
}


//************ getters ***************//

bool ofxVolumetrics::isInitialized()
{
    return bIsInitialized;
}
int ofxVolumetrics::getVolumeWidth()
{
    return volWidth;
}
int ofxVolumetrics::getVolumeHeight()
{
    return volHeight;
}
int ofxVolumetrics::getVolumeDepth()
{
    return volDepth;
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


//************ setters ***************//


void ofxVolumetrics::setXyQuality(float q)
{
    float oldQuality = quality.x;
    quality.x = MAX(q,0.01);
	
    updateRenderDimentions();
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
void ofxVolumetrics::setVolumeTextureFilterMode(GLint filterMode) {
    if(filterMode != GL_NEAREST && filterMode != GL_LINEAR) return;
	
    volumeTexture.bind();
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filterMode);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, filterMode);
    volumeTexture.unbind();
}

void ofxVolumetrics::setSlices(ofVec3f* _planes)
{
    planeCoords	= _planes;
}

void ofxVolumetrics::setRayPlane(ofPlane* _rayPlane)
{
    rayPlane = _rayPlane;
}




