#include "ofxIM920s.h"

ofxIM920s::ofxIM920s() {
}

ofxIM920s::~ofxIM920s() {
	close();
}

void ofxIM920s::setup(string comPort) {
	// IM920s rate is this
	const int baudRate = 19200;

	serial.setup(comPort, baudRate);
	if (serial.isInitialized()) {
		startThread();
		setMode(Hex);
	}
}

void ofxIM920s::close() {
	threadStopFlag = true;
	while (isThreadRunning()) {
		ofSleepMillis(1);
	}
	serial.close();
}

void ofxIM920s::threadedFunction() {
	ofLogNotice(tag) << "Start thread.";
	threadStopFlag = false;
	while (!threadStopFlag) {
		updateSerial();
	}
}

bool ofxIM920s::setWritable() {
	sendCommand("ENWR");
	return waitForResponse();
}

bool ofxIM920s::setMode(Mode _mode, bool _async) {
	mode = _mode;
	switch (mode) {
	case Hex: sendCommand("DCIO"); break;
	case Character: sendCommand("ECIO"); break;
	}

	if (_async) return true;
	else return waitForResponse();
}

ofxIM920s::Mode ofxIM920s::getMode() {
	return mode;
}

bool ofxIM920s::setNodeId(unsigned int id, bool _async) {
	sendCommand("STNN " + ofToString(id, 4, '0'));

	if (_async) return true;
	else {
		bool result = waitForResponse();
		if (result) {
			nodeId = id;
		}
		return result;
	}
}

unsigned int ofxIM920s::getNodeId() {
	return nodeId;
}

bool ofxIM920s::readNodeId(bool _async) {
	if (_async) return true;
	else return waitForResponse();
}

bool ofxIM920s::sendMessage(unsigned int _nodeId, string _msg, bool _async) {
	string payload = "";
	if (mode == Hex) {
		for (int i = 0; i < _msg.length(); ++i) {
			payload += ofToHex(_msg[i]);
			if (i + 1 < _msg.length()) payload += ',';
		}
	}
	else if (mode == Character) {
		payload = _msg;
	}

	sendCommand("TXDU " + ofToString(_nodeId, 4, '0') + ',' + payload);
	if (_async) return true;
	else return waitForResponse();
}

bool ofxIM920s::bloadcastMessage(string _msg, bool _async) {
	sendCommand("TXDA " + _msg);
	if (_async) return true;
	else return waitForResponse();
}

void ofxIM920s::updateSerial() {
	if (serial.isInitialized()) {
		// received
		while (serial.available()) {
			serialReceived += serial.readByte();
			if (serialReceived.length() > 2 && serialReceived.substr(serialReceived.length() - 2, 2) == "\r\n") {
				string msg = serialReceived.substr(0, serialReceived.length() - 2);
				messageReceived(msg);
				serialReceived = "";
			}
		}

		while (!commandQueue.empty()) {
			commandQueueMutex.lock();

			// ignore end of 2 charactoers, because it is "\r\n".
			ofLogNotice(tag) << "send message: " << string(commandQueue.front()).substr(0, commandQueue.size() - 2);

			serial.writeBytes(commandQueue.front());
			commandQueue.pop();

			commandQueueMutex.unlock();
		}
	}
	else {
		ofLogWarning(tag) << "Serial is not initialized.";
		threadStopFlag = false;
	}
}

void ofxIM920s::messageReceived(string _msg) {
	if (_msg == "OK") {
		ofLogNotice(tag) << "Response: OK";
		receivedResponse = _msg;
	}
	else if (_msg == "NG") {
		ofLogWarning(tag) << "Response: NG";
		receivedResponse = _msg;
	}
	else {
		auto splitColon = ofSplitString(_msg, ":");
		if (splitColon.size() == 2) {
			auto header = ofSplitString(splitColon[0], ",");
			if (header.size() == 3) {
				int firstNum = ofToInt(header[0]);
				int sendNode = ofToInt(header[1]);
				string msgType = header[2];
				string payload = "";
				if (mode == Hex) {
					for (auto& hex : ofSplitString(splitColon[1], ",")) {
						payload += ofHexToChar(hex);
					}
				}
				else if (mode == Character) {
					payload = splitColon[1];
				}

				ofLogNotice(tag) << "Message from: " << sendNode << " payload: " << payload;
			}
			else {
				ofLogWarning(tag) << "Invalid header \"" << splitColon[0] << "\"";
			}
		}
		else {
			ofLogWarning(tag) << "Invalid data \"" << _msg << "\"";
		}
	}
}

void ofxIM920s::sendCommand(string _msg) {
	receivedResponse == "";
	string msgWithCRLF = _msg + "\r\n";

	if (serial.isInitialized()) {
		commandQueueMutex.lock();
		commandQueue.push(ofBuffer(msgWithCRLF.c_str(), msgWithCRLF.length()));
		commandQueueMutex.unlock();
	}
	else {
		ofLogWarning(tag) << "Serial is not initialized.";
		threadStopFlag = false;
	}
}

bool ofxIM920s::waitForResponse() {
	const unsigned int timeoutMillis = 1000;
	unsigned int start = ofGetElapsedTimeMillis();

	while (receivedResponse == "") {
		ofSleepMillis(1);

		// check timeout
		if (ofGetElapsedTimeMillis() - start > timeoutMillis) {
			ofLogWarning(tag) << "No response, timeout.";
			return false;
		}
	}
	bool result = receivedResponse == "OK";
	receivedResponse = "";
	return result;
}
