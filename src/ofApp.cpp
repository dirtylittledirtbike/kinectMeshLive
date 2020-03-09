#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofBackground(80);
    
    // Open the default connect.
    vector <ofxKinectV2::KinectDeviceInfo> deviceList = ofxKinectV2().getDeviceList();
    
    // Check to see if there is a KinectV2 attached.
    if (deviceList.size() > 0){
        std::cout<< "Success: Kinect V2 found" << std::endl;
    } else {
        std::cerr<< "Failure: No Kinect V2 found" << std::endl;
        return;
    }
    
    // Reset min/max distances (as per ofxKinectV2).
    //    kinect.params.getFloat("minDistance").set(1200);
    //    kinect.params.getFloat("maxDistance").set(12000);
    
    kinect.open(deviceList[0].serial);
    
    int width = kinect.getRegisteredPixels().getWidth();
    int height = kinect.getRegisteredPixels().getHeight();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    std::size_t meshRows = kinect.getRegisteredPixels().getHeight();
    std::size_t meshCols = kinect.getRegisteredPixels().getWidth();
    
    float meshWidth = kinect.getRegisteredPixels().getWidth();
    float meshHeight = kinect.getRegisteredPixels().getHeight();
    
    std::size_t numCoordinates = 0;
    glm::vec3 avgPosition = { 0, 0, 0 };
    
    mesh.setMode(OF_PRIMITIVE_LINES);
    
    kinect.update();
    
    if (kinect.isFrameNew()){
        
        mesh.clear();
        
        for (std::size_t row = 0; row < meshRows; row++){
            for (std::size_t col = 0; col < meshCols; col++){
                
                std::size_t depthX = col;
                std::size_t depthY = row;
                
                // 3D Positions in space.
                ofColor color = kinect.getRegisteredPixels().getColor(col, row);
                mesh.addColor(color);
                
                float x = ofMap(col, 0, meshCols, 0, meshWidth);
                float y = ofMap(row, 0, meshRows, 0, meshHeight);
                float z = ofMap(kinect.getRawDepthPixels()[row * meshCols + col], kinect.minDistance, kinect.maxDistance, -600, 600, true);
                
                glm::vec3 position = {x + ofRandom(10) * 0.75, y + ofRandom(10) * 0.75, z + ofRandom(10) * 0.75};
                mesh.addVertex(position);
                
//                glm::vec3 noise = {ofRandom(0,100000), ofRandom(0,100000), ofRandom(0,100000)};
//                offsets.push_back(noise);
                
            }
        }
        
        
        //connect the vertices to generate triangles for mesh
        for (std::size_t row = 0; row < meshRows; row++){
            for (std::size_t col = 0; col < meshCols; col++){
                if (col < (meshCols - 1) && row < (meshRows - 1)){
                    
                    ofIndexType currentIndex   = (row     ) * meshCols + (col     );
                    ofIndexType eastIndex      = (row + 1) * meshCols + (col     );
                    ofIndexType southEastIndex = (row + 1) * meshCols + (col + 1);
                    ofIndexType southIndex     = (row     ) * meshCols + (col + 1);
                    
                    glm::vec3 currentPosition = mesh.getVertices()[currentIndex];
                    glm::vec3 eastPosition = mesh.getVertices()[eastIndex];
                    glm::vec3 southPosition = mesh.getVertices()[southIndex];
                    glm::vec3 southEastPosition = mesh.getVertices()[southEastIndex];
                    
                    float distance1 = glm::distance(currentPosition.z, eastPosition.z);
                    float distance2 = glm::distance(currentPosition.z, southPosition.z);
                    float distance3 = glm::distance(currentPosition.z, eastPosition.z);
                    float distance4 = glm::distance(currentPosition.z, southEastPosition.z);
                    float avgDist = (distance1 + distance2 + distance3 + distance4)/4;
                    
                    if (avgDist < 10 && currentPosition.z < -500 && currentPosition.z > -600){
                        
                        avgPosition += currentPosition;
                        numCoordinates++;
                        
                        // Triangle 0
                        mesh.addIndex(currentIndex);
                        mesh.addIndex(eastIndex);
                        mesh.addIndex(southEastIndex);
                        
                        // Triangle 1
                        mesh.addIndex(currentIndex);
                        mesh.addIndex(southEastIndex);
                        mesh.addIndex(southIndex);
                        
                    }
                }
            }
        }
        
        
        rgbPixels = kinect.getRegisteredPixels();
        rgbTexture.loadData(rgbPixels);
        
        //fix mesh display
        avgPosition /= double(numCoordinates);
        
        for (auto& vertex: mesh.getVertices()){
            vertex -= avgPosition;
        }
        
        
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){

    camera.begin();
    ofEnableDepthTest();
    ofPushMatrix();
    ofScale(-1, -1);
    mesh.draw();
    ofPopMatrix();
    ofDisableDepthTest();
    camera.end();
    
//    rgbTexture.draw(0, 0, rgbTexture.getWidth(), rgbTexture.getHeight());

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
