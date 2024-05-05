#pragma once

#include "ofMain.h"

#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_GENERATION
#define MA_NO_ENCODING
#define MA_NO_MP3
#define MA_NO_FLAC
#define MA_NO_SDL
#include "miniaudio.h"

#define FOREGROUND_COLOR ofColor(8)
#define BACKGROUND_COLOR ofColor(224, 224, 255)

// #define FOREGROUND_COLOR ofColor(224, 224, 255)
// #define BACKGROUND_COLOR ofColor(8)

#define HIGHLIGHT_COLOR  ofColor(255, 64, 64)

#define TILE_WIDTH   2
#define TILE_GAP     0.5

#define USER_Y_LEVEL 1.33

class ofApp : public ofBaseApp{

	public:
		~ofApp();

		void setup();
		// void update();
		void draw();

		void keyPressed(int key);
		// void keyReleased(int key);
		// void mouseMoved(int x, int y );
		// void mouseDragged(int x, int y, int button);
		// void mousePressed(int x, int y, int button);
		// void mouseReleased(int x, int y, int button);
		// void mouseEntered(int x, int y);
		// void mouseExited(int x, int y);
		// void windowResized(int w, int h);
		// void dragEvent(ofDragInfo dragInfo);
		// void gotMessage(ofMessage msg);

		void updateSoundscape();
		void playRandom();

		double timeSinceLastPlayed = 0;

		double deltaTime = 0;

		ofEasyCam cam;

		ofLight light;

		ofMesh tiles[TILE_WIDTH*TILE_WIDTH] = {};
		struct {
			ofMesh mesh;
			glm::vec2 pos;
		} user;
};
