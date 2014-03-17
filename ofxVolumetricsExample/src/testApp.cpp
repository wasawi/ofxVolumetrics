#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
	
    ofSetFrameRate(60);
    background.allocate(1024,768,OF_IMAGE_COLOR);
    background.loadImage("background.png");
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);

    cam.setDistance(1000);
    cam.enableMouseInput();
	
	initVolume();
}

//--------------------------------------------------------------
void testApp::initVolume()
{
	//Volume
	voxelSize	= ofVec3f(1);
	volPos		= ofVec3f(0);
	volSize		= ofVec3f(0);
	volOffset	= ofVec3f(0);
	uiClamp		= ofVec3f(0);
	
	//intersection plane
	rayPlane.setCenter(ofVec3f(0));
	rayPlane.setScale(ofVec3f(1));
	rayPlane.setNormal(ofVec3f(0.0f, 0.0f, 1.0f));
	rayPlane.setUp(ofVec3f(0.0f, 1.0f, 0.0f));
	rayPlane.setInfinite(false);

	
	FBOq		= 1;
	Zq			= 2;
	thresh		= 0;
	density		= 1;
	dithering	= 0;
	
	
	imageSequence.init("volumes/Colin27T1_tight/IM-0001-0",3,".tif", 0);
	//	imageSequence.init("volumes/talairach_nii/IM-0001-0",3,".tif", 0);
	
	// calculate volume size
	volWidth	= imageSequence.getWidth();
    volHeight	= imageSequence.getHeight();
    volDepth	= imageSequence.getSequenceLength();
	float size	= ofGetHeight();
	ofVec3f volumeSize = voxelSize * ofVec3f(volWidth,volHeight,volDepth);
    float maxDim = max(max(volumeSize.x, volumeSize.y), volumeSize.z);
    volSize = volumeSize * size / maxDim;
	
	
	ofLogNotice("vizManager") << "setting up volume data buffer at " << volWidth << "x" << volHeight << "x" << volDepth;
    volumeData = new unsigned char[volWidth*volHeight*volDepth];
	
	//fill out the array pixel in white for easy debugging
	for (int i=0; i<volWidth*volHeight*volDepth; i++ )
	{
		volumeData[i]= (unsigned char) 255;
	}
	
	// fill my array with pixels
    for(int z=0; z<volDepth; z++)
    {
        imageSequence.loadFrame(z);
		int gradient = 0;
		for(int y=0; y<volHeight; y++)
        {
			for(int x=0; x<volWidth; x++)
			{
				if (x<volWidth && y<volHeight)
				{																// get values from image
					int i = ((x + volWidth*y) + z*volWidth*volHeight);			// the pointer position at Array
					int sample = imageSequence.getPixels()[x+y*volWidth];		// the pixel on the image
					volumeData[i] = sample;
					//					ofLogVerbose("vizManager") << sample << " ";
				}
            }
        }
    }
	
	// Init Volume
    myVolume.setup(volWidth, volHeight, volDepth, voxelSize, true, &cam);
	myVolume.updateVolumeData(volumeData, volWidth, volHeight, volDepth, 0, 0, 0);
    myVolume.setRenderSettings(FBOq, Zq, density, thresh);
	myVolume.setVolumeTextureFilterMode(GL_LINEAR);
	myVolume.setSlices(&uiClamp);
	myVolume.setRayPlane(&rayPlane);

}

//--------------------------------------------------------------
void testApp::update()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofSetColor(255,255,255,255);
    background.draw(0,0,ofGetWidth(),ofGetHeight());

    cam.begin();
    myVolume.updateVolume(volPos, volSize, 0);
    cam.end();
	myVolume.draw(0, 0, ofGetWidth(), ofGetHeight());
	
	drawLabels();
}

//--------------------------------------------------------------
void testApp::drawLabels()
{
    ofSetColor(0,0,0,64);
    ofRect(0,0,270,90);
    ofSetColor(255,255,255,255);
	
    ofDrawBitmapString("volume dimensions: " + ofToString(myVolume.getVolumeWidth()) + "x" + ofToString(myVolume.getVolumeHeight()) + "x" + ofToString(myVolume.getVolumeDepth()) + "\n" +
                       "FBO quality (q/Q): " + ofToString(myVolume.getRenderWidth()) + "x" + ofToString(myVolume.getRenderHeight()) + "\n" +
                       "Z quality (z/Z):   " + ofToString(myVolume.getZQuality()) + "\n" +
                       "Threshold (t/T):   " + ofToString(myVolume.getThreshold()) + "\n" +
                       "Density (d/D):     " + ofToString(myVolume.getDensity()) + "\n" +
                       "Filter mode (l/n): " + (linearFilter?"linear":"nearest"),20,20);
	

}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{

    switch(key)
    {
    case 't':
        myVolume.setThreshold(myVolume.getThreshold()-0.01);
        break;
    case 'T':
        myVolume.setThreshold(myVolume.getThreshold()+0.01);
        break;
    case 'd':
        myVolume.setDensity(myVolume.getDensity()-0.01);
        break;
    case 'D':
        myVolume.setDensity(myVolume.getDensity()+0.01);
        break;
    case 'q':
        myVolume.setXyQuality(myVolume.getXyQuality()-0.01);
        break;
    case 'Q':
        myVolume.setXyQuality(myVolume.getXyQuality()+0.01);
        break;
    case 'z':
        myVolume.setZQuality(myVolume.getZQuality()-0.01);
        break;
    case 'Z':
        myVolume.setZQuality(myVolume.getZQuality()+0.01);
        break;
    case 'l':
        myVolume.setVolumeTextureFilterMode(GL_LINEAR);
        linearFilter = true;
        break;
    case 'n':
        myVolume.setVolumeTextureFilterMode(GL_NEAREST);
        linearFilter = false;
        break;
    case OF_KEY_UP:
        cam.getTarget().boom(-5);
        break;
    case OF_KEY_DOWN:
        cam.getTarget().boom(5);
        break;
    case OF_KEY_LEFT:
        cam.getTarget().truck(-5);
        break;
    case OF_KEY_RIGHT:
        cam.getTarget().truck(5);
        break;
    }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}
