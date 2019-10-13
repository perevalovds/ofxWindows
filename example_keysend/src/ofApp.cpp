#include "ofApp.h"
#include "ofxWindows.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetWindowShape(200, 100);

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	float time = ofGetElapsedTimef();
	if (time - time_ >= 1) {
		time_ = time;
		string window_class = "Notepad";
		char c = k + '0';
		bool result = ofxWindows::SendKeyboardByWindowClass(window_class, c);
		if (result) {
			cout << "Sent " << c << " to " << window_class << endl;
		}
		else {
			cout << "Can't sent symbol, may be window with class " << window_class << " doesn't exists"<< endl;
			cout << "To check this, run unitilies/winlister" << endl;
		}
		k = (k+1) % 10;
	}

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
