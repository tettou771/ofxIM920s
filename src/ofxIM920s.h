#ifndef __ofxim920s_h__
#define __ofxim920s_h__

#include "ofMain.h"
#include <glm\detail\type_vec3.hpp>

class ofxIM920s : public ofThread {
public:
	enum Mode {
		Hex, // DCIO
		Character // ECIO
	};

	struct EventArgs {
		unsigned int from;
		string type;
		string payload;
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

	ofEvent<EventArgs> events;

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

	void notifyEvent(int _from, string _type, string _payload);
	void notifyEvent(EventArgs _args);

	const string tag = "ofxIM920s";

	unsigned int nodeId;
	Mode mode;
};

#endif