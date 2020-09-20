#ifndef __ofxim920s_h__
#define __ofxim920s_h__

#include "ofMain.h"

class ofxIM920s : public ofThread {
public:
	enum Mode {
		Hex, // DCIO
		Character // ECIO
	};


	ofxIM920s();
	~ofxIM920s();
	
	void setup(string comPort);
	void close();
	void threadedFunction() override;

	// control the device
	bool setWritable();
	bool setMode(Mode _mode, bool _async = false);
	Mode getMode();
	bool setNodeId(unsigned int id, bool _async = false);
	unsigned int getNodeId();
	bool readNodeId(bool _async = false);
	bool sendMessage(unsigned int _nodeId, string _msg, bool _async = false);
	bool bloadcastMessage(string _msg, bool _async = false);

private:
	void updateSerial();
	void messageReceived(string _msg);
	void sendCommand(string _cmd);
	bool waitForResponse();
	bool threadStopFlag;

	queue<ofBuffer> commandQueue;
	ofMutex commandQueueMutex;

	ofSerial serial;
	string serialReceived;
	string receivedResponse;

	const string tag = "ofxIM920s";

	unsigned int nodeId;
	Mode mode;
};

#endif