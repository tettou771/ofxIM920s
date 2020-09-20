#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofLogToConsole();

	// connect to device
	// com port name is depend on your environment, please change it.
	// 
	// ofxIM920s is threaded, then it don't need update yourself.
	device.setup("COM6");
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofDrawBitmapString("Please see to console", 100, 100);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	string msg = "KeyTyped/" + key;

	while (true) {
		bool result = device.sendMessage(2, msg);

		// response OK
		if (result) {
			break;
		}

		// response NG, then retry
		else {
			ofSleepMillis(100);
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	string msg = "MouseMoved/" + ofToString(x) + "/" + ofToString(y);

	// async option
	// true (default):
	//     wait response OK/NG
	//     and return true/false
	// false:
	//	   don't wait for response
	//     always return true
	//     for high traffic and low trusted

	bool async = true;
	device.sendMessage(2, msg, async);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
