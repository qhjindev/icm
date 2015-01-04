package com.tnsoft.icm.icm4j.protocol;


public interface IcmProtocol {

	int MESSAGE_HEAD_SIZE = 5;

	byte TYPE_REQUEST = 0;
	byte TYPE_RESPONSE = 1;

	byte NO_RESPONSE = 0;
	byte HAS_RESPONSE = 1;

	int RESPONSE_OK = 0;
	int RESPONSE_UNKNOWN_ERROR = -1;

}
