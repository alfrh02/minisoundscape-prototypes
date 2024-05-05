#include "ofApp.h"

/*

This template openFrameworks project runs miniaudio's high level API on top of openFrameworks, rather than the alternative of having miniaudio use openFramework's audio.
This is because having miniaudio run on top is simply easier and works just the same, though a proper openFrameworks implmentation may grant greater
flexibility within openFrameworks and shouldn't be discounted.

*/

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#define MA_NO_GENERATION
#define MA_NO_ENCODING

#define MA_NO_MP3
#define MA_NO_FLAC

#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#define MA_ENABLE_JACK

#define MA_SOUND_FLAG_NO_PITCH
#define MA_SOUND_FLAG_NO_SPATIALIZATION

#define MA_NO_SDL

static ma_engine engine;
static ma_engine_config engineConfig;

static ma_sound sound;

unsigned short lineCount = 0;
unsigned short lineFidelity = 8;
float lineArr[8] = {
	0, 0, 0, 0, 0, 0, 0, 0
};

void engine_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_engine_read_pcm_frames(&engine, pOutput, frameCount, NULL);
	lineArr[lineCount] = *(float*)pOutput;
	lineCount = (lineCount + 1) % lineFidelity;
}

//--------------------------------------------------------------
void ofApp::setup(){
	engineConfig = ma_engine_config_init();
	engineConfig.dataCallback = engine_data_callback;

	ma_result result = ma_engine_init(&engineConfig, &engine);
	if (result != MA_SUCCESS) {
		printf("Failed to initialise engine.\n");
	}

	result = ma_sound_init_from_file(&engine, ofToDataPath("chirp.wav", true).c_str(), 0, NULL, NULL, &sound);
	if (result != MA_SUCCESS) {
		printf("Failed to initialise sound.\n");
	}

	ofNoFill();
	ofBackground(224);
}

//--------------------------------------------------------------
void ofApp::update(){
	deltaTime += ofGetLastFrameTime();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(ofColor(24));
	ofBeginShape();
		for (size_t i = 0; i < lineFidelity; i++) {
			ofVertex(i * ofGetWidth() / lineFidelity, (lineArr[i] + 1) * ofGetHeight() / 2);
		}
		ofVertex(ofGetWidth(), ofGetHeight() / 2);
	ofEndShape();
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	ma_sound_start(&sound);
}