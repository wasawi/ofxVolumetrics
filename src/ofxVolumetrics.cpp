

#include "ofxVolumetrics.h"
#include "ofMain.h"
#define STRINGIFY( A) #A
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
	bNewCode		=false;

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

void ofxVolumetrics::destroy()
{
    volumeShader.unload();
    volumeTexture.clear();
    //    fboBackground.destroy();
    //    fboRender.destroy();
}

void ofxVolumetrics::setup(int w, int h, int d, ofVec3f _voxelRatio, bool usePowerOfTwoTexSize)
{
    string vertexShader = STRINGIFY(
                                    varying vec3 cameraPosition;
                                    void main()
                                    {
                                        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                                        gl_TexCoord[0] = gl_MultiTexCoord0; //poop
                                        cameraPosition = (gl_ModelViewMatrixInverse * vec4(0.,0.,0.,1.)).xyz;
                                    }); // END VERTEX SHADER STRINGIFY

    string fragmentShader = STRINGIFY((#extension GL_ARB_texture_rectangle : enable \n
                                        varying vec3 cameraPosition;

                                        uniform sampler3D volume_tex;
                                        uniform vec3 vol_d;
                                        uniform vec3 vol_d_pot;
                                        uniform vec2 bg_d;
                                        uniform float zoffset;
                                        uniform float quality;
                                        uniform float threshold;
                                        uniform float density;

                                        struct Ray {
                                            vec3 Origin;
                                            vec3 Dir;
                                        };

                                        struct BoundingBox {
                                            vec3 Min;
                                            vec3 Max;
                                        };

                                        bool IntersectBox(Ray r, BoundingBox box, out float t0, out float t1)
                                        {
                                            vec3 invR = 1.0 / r.Dir;
                                            vec3 tbot = invR * (box.Min-r.Origin);
                                            vec3 ttop = invR * (box.Max-r.Origin);
                                            vec3 tmin = min(ttop, tbot);
                                            vec3 tmax = max(ttop, tbot);
                                            vec2 t = max(tmin.xx, tmin.yz);
                                            t0 = max(t.x, t.y);
                                            t = min(tmax.xx, tmax.yz);
                                            t1 = min(t.x, t.y);
                                            return t0 <= t1;
                                        }

                                        void main()
                                        {

                                            vec3 minv = vec3(0.)+1./vol_d_pot;
                                            vec3 maxv = (vol_d/vol_d_pot)-1./vol_d_pot;
                                            vec3 vec;
                                            vec3 vold = (maxv-minv)*vol_d;
                                            float vol_l = length(vold);

                                            vec4 col_acc = vec4(0,0,0,0);
                                            vec3 zOffsetVec = vec3(0.0,0.0,zoffset/vol_d_pot.z);
                                            vec3 backPos = gl_TexCoord[0].xyz;
                                            vec3 lookVec = normalize(backPos - cameraPosition);


                                            Ray eye = Ray( cameraPosition, lookVec);
                                            BoundingBox box = BoundingBox(vec3(0.),vec3(1.));

                                            float tnear, tfar;
                                            IntersectBox(eye, box, tnear, tfar);
                                            if(tnear < 0.15) tnear = 0.15;
                                            if(tnear > tfar) discard;

                                            vec3 rayStart = (eye.Origin + eye.Dir * tnear)*(maxv-minv)+minv;//vol_d/vol_d_pot;
                                            vec3 rayStop = (eye.Origin + eye.Dir * tfar)*(maxv-minv)+minv;//vol_d/vol_d_pot;

                                            vec3 dir = rayStop - rayStart; // starting position of the ray

                                            vec = rayStart;
                                            float dl = length(dir);
                                            if(dl == clamp(dl,0.,vol_l)) {
                                                int steps = int(floor(length(vold * dir) * quality));
                                                vec3 delta_dir = dir/float(steps);
                                                vec4 color_sample;
                                                float aScale =  density/quality;

                                                float random = fract(sin(gl_FragCoord.x * 12.9898 + gl_FragCoord.y * 78.233) * 43758.5453);
                                                vec += delta_dir * random;

                                                //raycast
                                                for(int i = 0; i < steps; i++)
                                                {
                                                    vec3 vecz = vec + zOffsetVec;
                                                    if(vecz.z > maxv.z) vecz.z-=maxv.z;
                                                    color_sample = texture3D(volume_tex, vecz);
                                                    if(color_sample.a > threshold) {

                                                        float oneMinusAlpha = 1. - col_acc.a;
                                                        color_sample.a *= aScale;
                                                        col_acc.rgb = mix(col_acc.rgb, color_sample.rgb * color_sample.a, oneMinusAlpha);
                                                        col_acc.a += color_sample.a * oneMinusAlpha;
                                                        col_acc.rgb /= col_acc.a;
                                                        if(col_acc.a >= 1.0) {
                                                            break; // terminate if opacity > 1
                                                        }
                                                    }
                                                    vec += delta_dir;
                                                }
                                            }
                                            // export the rendered color
                                            gl_FragColor = col_acc;

                                        } )); // END FRAGMENT SHADER STRINGIFY

    // For whatever reason, the stringify macro takes the fragment shader code as 2 arguments,
    // wrapping it in () makes it compile, so trim them off
    fragmentShader = fragmentShader.substr(1,fragmentShader.size()-2);

    volumeShader.unload();
    volumeShader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    volumeShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    volumeShader.linkProgram();

    bIsPowerOfTwo = usePowerOfTwoTexSize;

    volWidthPOT = volWidth = renderWidth = w;
    volHeightPOT = volHeight = renderHeight = h;
    volDepthPOT = volDepth = d;

    if(bIsPowerOfTwo){
        volWidthPOT = ofNextPow2(w);
        volHeightPOT = ofNextPow2(h);
        volDepthPOT = ofNextPow2(d);

        ofLogVerbose() << "ofxVolumetrics::setup(): Using power of two texture size. Requested: " << w << "x" <<h<<"x"<<d<<". Actual: " << volWidthPOT<<"x"<<volHeightPOT<<"x"<<volDepthPOT<<".";
    }

    fboRender.allocate(w, h, GL_RGBA);
    volumeTexture.allocate(volWidthPOT, volHeightPOT, volDepthPOT, GL_RGBA);
    if(bIsPowerOfTwo){
        // if using cropped power of two, blank out the extra memory
        unsigned char * data;
        data = new unsigned char[volWidthPOT*volHeightPOT*volDepthPOT*4];
        memset(data,0,volWidthPOT*volHeightPOT*volDepthPOT*4);
        volumeTexture.loadData(data,volWidthPOT, volHeightPOT, volDepthPOT, 0,0,0,GL_RGBA);
    }
    voxelRatio = _voxelRatio;
    bIsInitialized = true;
	
	// added so that I can use my render
	setCubeSize(ofVec3f(volWidth, volHeight, volDepth), _voxelRatio);
	updateRenderDimentions();
}
/*
void ofxVolumetrics::destroy()
{
    volumeShader.unload();
//    fboBackground.destroy();
//    fboRender.destroy();
    volumeTexture.clear();

    volWidth = renderWidth = 0;
    volHeight = renderHeight = 0;
    volDepth = 0;
    bIsInitialized = false;
}
*/

//--------------------------------------------------------------
void ofxVolumetrics::setup(ofxVolume* _volume, ofVec3f _voxelRatio, bool usePowerOfTwoTexSize, GLint internalformat)
{
    // FIXING GUI ERROR, https://github.com/openframeworks/openFrameworks/issues/1515
    GLuint clearErrors = glGetError();
	
	// load shader
//	volumeShader.load("shaders/ofxVolumetrics");
	volumeShader.load("../../../../ofxVolumetrics/src/shaders/ofxVolumetricsOriginal");
	
	// load volume to 3dTexture
    setVolume(_volume, usePowerOfTwoTexSize, internalformat);
	
	setCubeSize(vol->getSize(), _voxelRatio);
//	setCubeSize(ofVec3f(151,188,154), _voxelRatio);
	
    updateRenderDimentions();
    
	bIsInitialized = true;
	bNewCode = true;
}

//--------------------------------------------------------------
void ofxVolumetrics::setVolume(ofxVolume* _volume, bool usePowerOfTwoTexSize, GLint internalformat)
{
//    volumeTexture.clear();
    vol = _volume;
	
	volWidthPOT		= ofNextPow2(vol->getWidth());
	volHeightPOT	= ofNextPow2(vol->getHeight());
	volDepthPOT		= ofNextPow2(vol->getDepth());
	
	volWidth		= vol->getWidth();
	volHeight		= vol->getHeight();
	volDepth		= vol->getDepth();
	
/*	volWidth		= 151;
	volHeight		= 188;
	volDepth		= 154;
*/
//	cout <<"vol dim		" << volWidth<<"x"<<volHeight<<"x"<<volDepth<<".\n";
//	cout <<"volPot dim	" << volWidthPOT<<"x"<<volHeightPOT<<"x"<<volDepthPOT<<".\n";
	
    bIsPowerOfTwo = usePowerOfTwoTexSize;
    if(bIsPowerOfTwo){
		ofLogVerbose() << "ofxVolumetrics::setup(): Using power of two texture size. Requested: "
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
		volumeTexture.loadData(vol, volOffset, internalformat);
		setVolumeTextureFilterMode(filterMode);
    }else{
		ofLogVerbose() << "ofxVolumetrics::setup(): Requested: "
		<< vol->getWidth() << "x" <<vol->getHeight()<<"x"<<vol->getDepth()<<".\n";
        volumeTexture.allocate(vol->getSize(), internalformat);
        volumeTexture.loadData(vol, volOffset, internalformat);

		// old style loading
//		volumeTexture.allocate(volWidth, volHeight, volDepth, GL_RGBA);
//		volumeTexture.loadData(vol->getVoxels(), vol->getWidth(), vol->getHeight(), vol->getDepth(), 0, 0, 0, GL_RGBA);
		setVolumeTextureFilterMode(filterMode);
    }
}

//--------------------------------------------------------------
void ofxVolumetrics::setup(unsigned char * _data, ofVec3f _volSize, ofVec3f _voxelRatio, bool usePowerOfTwoTexSize, GLint internalformat)
{
	// FIXING GUI ERROR, https://github.com/openframeworks/openFrameworks/issues/1515
	GLuint clearErrors = glGetError();

//	volumeShader.load("shaders/ofxVolumetrics");
	volumeShader.load("../../../../ofxVolumetrics/src/shaders/ofxVolumetricsOriginal");

	voxelRatio = _voxelRatio;
	setVolume(_data, _volSize, usePowerOfTwoTexSize, internalformat);
	
	float size  = ofGetHeight();
	ofVec3f volumeSize = voxelRatio * ofVec3f(vol->getWidth(),vol->getHeight(),vol->getDepth());
	float maxDim = max(max(volumeSize.x, volumeSize.y), volumeSize.z);
	cubeSize = volumeSize * size / maxDim;

    updateRenderDimentions();
    bIsInitialized = true;
	bNewCode = true;
}
//--------------------------------------------------------------
void ofxVolumetrics::setVolume(unsigned char * _data, ofVec3f _volSize, bool usePowerOfTwoTexSize, GLint internalformat)
{
	/*
    volumeTexture.clear();

	bIsPowerOfTwo = usePowerOfTwoTexSize;
    if(bIsPowerOfTwo){
		
		volPot.setSize(ofNextPow2(_volSize.x), ofNextPow2(_volSize.y), ofNextPow2(_volSize.z));
		ofLogVerbose() << "ofxVolumetrics::setup(): Using power of two texture size. Requested: "
		<< _volSize.x << "x" << _volSize.y <<"x"<< _volSize.z <<". Actual: "
		<< volPot.w<< "x" << volPot.h<<"x"<< volPot.d<<".\n";
		
		// if using cropped power of two, blank out the extra memory
		unsigned char * data;
		data = new unsigned char[(int)volPot.getVolume()*4];
		memset(data,0,(int)volPot.getVolume()*4);
		volumeTexture.allocate(volPot.getSize(), internalformat);
		//		 volumeTexture.loadData(d,volPot.getWidth(), volPot.getHeight(), volPot.getDepth(), 0,0,0,internalformat);
		//		 volumeTexture.loadData(d, volPot.size, volOffset, internalformat);
//		volumeTexture.loadData(vol, volOffset, internalformat);
		vol->setFromVoxels(data, _volSize.x, _volSize.y, _volSize.z, 4);
    }else{
		volPot.setSize(vol->getSize());
		volumeTexture.allocate( _volSize, internalformat);
		volumeTexture.loadData( _data, vol->getSize(), volOffset, internalformat);
    }*/
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

/*
//--------------------------------------------------------------
void ofxVolumetrics::update()
{
    updateRenderDimentions();
	cout <<"MY UPDATE*************************\n";
	cout <<"cubeSize		" << cubeSize<<".\n";
	cout <<"render		" << renderWidth<<"x"<< renderHeight<<".\n";
	cout <<"vol dim		" << volWidth<<"x"<<volHeight<<"x"<<volDepth<<".\n";
	cout <<"volPot dim	" << volWidthPOT<<"x"<<volHeightPOT<<"x"<<volDepthPOT<<".\n";
	
    GLfloat modl[16], proj[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, modl);
    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    GLint color[4];
    glGetIntegerv(GL_CURRENT_COLOR, color);
	
    ofVec3f scale,t;
    ofQuaternion a,b;
    ofMatrix4x4(modl).decompose(t, a, scale, b);
	
    GLint cull_mode;
    glGetIntegerv(GL_FRONT_FACE, &cull_mode);
    GLint cull_mode_fbo = (scale.x*scale.y*scale.z) > 0 ? GL_CCW : GL_CW;
	
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
	ofTranslate(cubePos.x - cubeSize.x/2, cubePos.y - cubeSize.y/2, cubePos.z - cubeSize.z/2);
    ofScale(cubeSize.x,cubeSize.y,cubeSize.z);
	
    //pass variables to the shader
    glActiveTexture(GL_TEXTURE1);
    volumeTexture.bind();
    volumeShader.setUniform1i("volume_tex", 1); // volume texture reference
    volumeTexture.unbind();
    glActiveTexture(GL_TEXTURE0);
	
    volumeShader.setUniform3f("vol_d", (float)volWidth, (float)volHeight, (float)volDepth); //dimensions of the volume texture
    volumeShader.setUniform3f("vol_d_pot", (float)volWidthPOT, (float)volHeightPOT, (float)volDepthPOT); //dimensions of the volume texture power of two
    volumeShader.setUniform2f("bg_d", (float)renderWidth, (float)renderHeight); // dimensions of the background texture
    volumeShader.setUniform1f("zoffset",0.0f); // used for animation so that we dont have to upload the entire volume every time
    volumeShader.setUniform1f("quality", quality.z); // 0 ... 1
    volumeShader.setUniform1f("density", density); // 0 ... 1
    volumeShader.setUniform1f("threshold", threshold);//(float)mouseX/(float)ofGetWidth());
	
    glFrontFace(cull_mode_fbo);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    drawRGBCube();
    glDisable(GL_CULL_FACE);
    glFrontFace(cull_mode);
	
	volumeShader.end();
    
	ofPopView();
	
	ofNoFill();
    drawCube(1.);
	ofDisableDepthTest();
	
    fboRender.end();
	
    glColor4iv(color);
    ofSetupScreenOrtho();//ofGetWidth(), ofGetHeight(),OF_ORIENTATION_DEFAULT,false,0,1000);
	fboRender.draw(0,ofGetHeight(),ofGetWidth(),-ofGetHeight());
}
*/



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
	//    volumeShader.setUniform1f("zoffset", zTexOffset);
    volumeShader.setUniform1f("quality", quality.z); // 0 ... 1
    volumeShader.setUniform1f("density", density); // 0 ... 1
    volumeShader.setUniform1f("dithering", dithering); // 0 ... 1
    volumeShader.setUniform1f("threshold", threshold);//(float)mouseX/(float)ofGetWidth());
	volumeShader.setUniform1f("zoffset",0.0f); // used for animation so that we dont have to upload the entire volume every time
//    volumeShader.setUniform1f("clipPlaneDepth", clipPlaneDepth);//-sizeFactor*planeCoords->y);
//    volumeShader.setUniform1f("azimuth", 360*azimuth);
//    volumeShader.setUniform1f("elevation", 360*elevation);
	
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
void ofxVolumetrics::drawCube(float size)
{
    float f = .99999;
	
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

void ofxVolumetrics::updateVolumeData(unsigned char * data, int w, int h, int d, int xOffset, int yOffset, int zOffset)
{
    volumeTexture.loadData(data, w, h, d, xOffset, yOffset, zOffset, GL_RGBA);
}

void ofxVolumetrics::drawVolume(float x, float y, float z, float size, int zTexOffset)
{
	
    ofVec3f volumeSize = voxelRatio * ofVec3f(volWidth,volHeight,volDepth);
    float maxDim = max(max(volumeSize.x, volumeSize.y), volumeSize.z);
    volumeSize = volumeSize * size / maxDim;

    drawVolume(x, y, z, volumeSize.x, volumeSize.y, volumeSize.z, zTexOffset);
}

void ofxVolumetrics::drawVolume(float x, float y, float z, float w, float h, float d, int zTexOffset)
{
    updateRenderDimentions();
	cout <<"HIS UPDATE-----------------------------\n";
	cout <<"cubeSize		" << cubeSize<<".\n";
	cout <<"render		" << renderWidth<<"x"<< renderHeight<<".\n";
	cout <<"vol dim		" << volWidth<<"x"<<volHeight<<"x"<<volDepth<<".\n";
	cout <<"volPot dim	" << volWidthPOT<<"x"<<volHeightPOT<<"x"<<volDepthPOT<<".\n";

    GLfloat modl[16], proj[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, modl);
    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    GLint color[4];
    glGetIntegerv(GL_CURRENT_COLOR, color);

    ofVec3f scale,t;
    ofQuaternion a,b;
    ofMatrix4x4(modl).decompose(t, a, scale, b);

    GLint cull_mode;
    glGetIntegerv(GL_FRONT_FACE, &cull_mode);
    GLint cull_mode_fbo = (scale.x*scale.y*scale.z) > 0 ? GL_CCW : GL_CW;

	ofEnableDepthTest();
    /* raycasting pass */
    fboRender.begin();
    volumeShader.begin();
    ofClear(0,0,0,0);

    //load matricies from outside the FBO
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(proj);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(modl);

    ofPushView();
	ofTranslate(cubePos.x - cubeSize.x/2, cubePos.y - cubeSize.y/2, cubePos.z - cubeSize.z/2);
    ofScale(cubeSize.x,cubeSize.y,cubeSize.z);

    //pass variables to the shader
    glActiveTexture(GL_TEXTURE1);
    volumeTexture.bind();
    volumeShader.setUniform1i("volume_tex", 1); // volume texture reference
    volumeTexture.unbind();
    glActiveTexture(GL_TEXTURE0);

    volumeShader.setUniform3f("vol_d", (float)volWidth, (float)volHeight, (float)volDepth); //dimensions of the volume texture
    volumeShader.setUniform3f("vol_d_pot", (float)volWidthPOT, (float)volHeightPOT, (float)volDepthPOT); //dimensions of the volume texture power of two
    volumeShader.setUniform2f("bg_d", (float)renderWidth, (float)renderHeight); // dimensions of the background texture
    volumeShader.setUniform1f("zoffset",zTexOffset); // used for animation so that we dont have to upload the entire volume every time
    volumeShader.setUniform1f("quality", quality.z); // 0 ... 1
    volumeShader.setUniform1f("density", density); // 0 ... 1
    volumeShader.setUniform1f("threshold", threshold);//(float)mouseX/(float)ofGetWidth());

    glFrontFace(cull_mode_fbo);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    drawRGBCube();
    glDisable(GL_CULL_FACE);
    glFrontFace(cull_mode);

	volumeShader.end();
    
	ofPopView();

	ofNoFill();
    drawCube(1.);
	ofDisableDepthTest();

    fboRender.end();



    glColor4iv(color);
    ofSetupScreenOrtho();//ofGetWidth(), ofGetHeight(),OF_ORIENTATION_DEFAULT,false,0,1000);
	fboRender.draw(0,ofGetHeight(),ofGetWidth(),-ofGetHeight());
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

//************ getters ***************//

bool ofxVolumetrics::isInitialized()
{
    return bIsInitialized;
}
int ofxVolumetrics::getVolumeWidth()
{
	if (bNewCode){
	return vol->getWidth();
	}else{
	return volWidth;
	}
}
int ofxVolumetrics::getVolumeHeight()
{
	if (bNewCode){
		return vol->getHeight();
	}else{
		return volHeight;
	}
}
int ofxVolumetrics::getVolumeDepth()
{
	if (bNewCode){
		return vol->getHeight();
	}else{
		return volHeight;
	}
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

