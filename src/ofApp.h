#pragma once

#include "ofMain.h"
#include "ofxKinectV2.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void keyPressed(int key);
    
    ofxKinectV2 kinect;
    ofTexture rgbTexture;
    ofPixels rgbPixels;
    ofMesh mesh, meshCopy;
    ofEasyCam camera;
    
    vector<glm::vec3> offsets;
    bool meshOn;
};
