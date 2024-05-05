#include "ofApp.h"

#define MS_VERBOSE
#include "minisoundscape.h"

static ma_engine engine;

int currentSoundscape = 0;

// static ms_soundscape ssRain;
// static ms_soundscape ssCity;
// static ms_soundscape* soundscapes[2] = { &ssRain, &ssCity };

static ms_soundscape* soundscapes[3] = {};

//--------------------------------------------------------------
// TODO: FIX UNINITIALISATION CAUSING SEGFAULTS, DOUBLE FREE()S
ofApp::~ofApp() {
	// soundscapes will uninitialise any soundbites that are "linked" to them
	// for (size_t i = SOUNDSCAPE_AMOUNT; i > 0; i--) {
	// 	ms_soundscape_uninit(soundscapes[i]);
	// }
}

//--------------------------------------------------------------
void ofApp::setup(){
	ma_result result = ma_engine_init(NULL, &engine);
	if (result != MA_SUCCESS) printf("Failed to initialise engine.\n");

	ma_sound* amb0 = new ma_sound;
	ma_sound* amb1 = new ma_sound;
	ma_sound* amb2 = new ma_sound;
	ma_sound* amb3 = new ma_sound;

	ma_sound_init_from_file(&engine, ofToDataPath("ambient/crickets.wav", false).c_str(), 0, NULL, NULL, amb0); ma_sound_set_volume(amb0, 0.1);
	ma_sound_init_from_file(&engine, ofToDataPath("ambient/city.wav",     false).c_str(), 0, NULL, NULL, amb1);
	ma_sound_init_from_file(&engine, ofToDataPath("ambient/garden.wav",   false).c_str(), 0, NULL, NULL, amb2); ma_sound_set_volume(amb2, 2.0);

	// ms_soundbite* sbThunder    = new ms_soundbite;
	ms_soundbite* sbCarDriving  = new ms_soundbite;
	ms_soundbite* sbSlide       = new ms_soundbite;
	ms_soundbite* sbSmallButton = new ms_soundbite;
	ms_soundbite* sbBird0       = new ms_soundbite;
	ms_soundbite* sbDoorOpening = new ms_soundbite;
	ms_soundbite* sbGlitch0     = new ms_soundbite;
	ms_soundbite* sbGlitch1     = new ms_soundbite;
	ms_soundbite* sbGlitch2     = new ms_soundbite;

	ms_soundbite_init(&engine, sbCarDriving,  ofToDataPath("soundbites/car-driving-by.wav", false)); // https://freesound.org/people/Frederik_Sunne/sounds/324138/
	ms_soundbite_set_volume(sbCarDriving, 0.15);

	ms_soundbite_init(&engine, sbSlide,       ofToDataPath("soundbites/slide_small_1.wav",  false)); // https://freesound.org/people/blaukreuz/sounds/459645/
	ms_soundbite_set_volume(sbSlide, 0.15);

	ms_soundbite_init(&engine, sbSmallButton, ofToDataPath("soundbites/small-button.wav",   false)); // https://freesound.org/people/grinesmr/sounds/710625/

	ms_soundbite_init(&engine, sbBird0,       ofToDataPath("soundbites/bird0.wav",          false)); // https://freesound.org/people/mrmicrowv/sounds/510187/
	ms_soundbite_set_volume(sbBird0, 0.15);

	ms_soundbite_init(&engine, sbDoorOpening, ofToDataPath("soundbites/door.wav",           false)); // https://freesound.org/people/pagancow/sounds/15419/
	ms_soundbite_set_volume(sbDoorOpening, 0.15);

	ms_soundbite_init(&engine, sbGlitch0,     ofToDataPath("soundbites/glitch/12908__sweet_trip__mm_clap-hi.wav",           false));
	ms_soundbite_init(&engine, sbGlitch1,     ofToDataPath("soundbites/glitch/12911__sweet_trip__mm_hat-cl.wav",           false));
	ms_soundbite_init(&engine, sbGlitch2,     ofToDataPath("soundbites/glitch/12916__sweet_trip__mm_kwik-mod-01.wav",           false));

	ms_soundscape* soundscape0 = new ms_soundscape;
	ms_soundscape* soundscape1 = new ms_soundscape;
	ms_soundscape* soundscape2 = new ms_soundscape;
	ms_soundscape* soundscape3 = new ms_soundscape;
	ma_sound* n = nullptr;

	ms_soundscape_init(soundscape0, "rain",   amb0,    1, sbSlide);
	ms_soundscape_init(soundscape1, "city",   amb1,    4, sbCarDriving, sbSlide, sbSmallButton, sbDoorOpening);
	ms_soundscape_init(soundscape2, "garden", amb2,    2, sbSmallButton, sbBird0);
	ms_soundscape_init(soundscape3, "glitch", n, 3, sbGlitch0, sbGlitch1, sbGlitch2);

	soundscapes[0] = soundscape0;
	soundscapes[1] = soundscape1;
	soundscapes[2] = soundscape2;
	soundscapes[3] = soundscape3;

	ms_soundscape_start(soundscapes[currentSoundscape]);

	ofBackground(BACKGROUND_COLOR);

	cam.setPosition(glm::vec3(0.0, 0.0, 2.0));
	cam.setNearClip(0.05);
	cam.setFarClip(100.0);

	// light.enable();

	for (size_t i = 0; i < TILE_WIDTH*TILE_WIDTH; i++) {
		tiles[i] = ofMesh::box(1.0, 0.1, 1.0, 1, 1);
	}

	user.mesh = ofMesh::sphere(0.33, 3);
	user.pos = glm::vec2(1.0, 1.0);
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(FOREGROUND_COLOR);

	stringstream s;
    // s << "Change mood with Q or E" << endl;
    s << "Move to different tiles using WASD. Current coords: " << user.pos.x << ", " << user.pos.y << endl;
    s << "Press space to play a soundbite" << endl;
    s << "Soundscape: (" + to_string(currentSoundscape + 1) + ") " + soundscapes[currentSoundscape]->name << endl;
    s << "---" << endl;

	for (ms_soundbite* sb : soundscapes[currentSoundscape]->soundbites) {
		if (ms_soundbite_is_playing(sb)) s << sb->name << endl;
	}

    ofDrawBitmapString(s.str().c_str(), 8, 12);

	ofEnableDepthTest();
	// ofEnableLighting();
	cam.begin();

	ofPushMatrix();
		ofTranslate(glm::vec3(
			-(((1 + TILE_GAP) * TILE_WIDTH) / 2) + 0.5,
			0,
			-(((1 + TILE_GAP) * TILE_WIDTH) / 2) + 0.5
		));
		float x = 0, z = 0;
		for (size_t i = 0; i < TILE_WIDTH * TILE_WIDTH; i++) {
			glm::vec3 pos = glm::vec3(x, 0, z);

			x += 1.0 + TILE_GAP;
			if (x >= (1.0 + TILE_GAP) * TILE_WIDTH) {
				x = 0;
				z += 1.0 + TILE_GAP;
			}

			ofPushMatrix();
				ofTranslate(pos);
				tiles[i].draw();
			ofPopMatrix();
		}

		ofSetColor(HIGHLIGHT_COLOR);
		ofPushMatrix();
			ofTranslate(glm::vec3(
				user.pos.x * (1.0 + TILE_GAP),
				USER_Y_LEVEL + sin((float)ofGetElapsedTimeMillis() / 1000) / 10,
				user.pos.y * (1.0 + TILE_GAP))
			);
			user.mesh.draw();
		ofPopMatrix();
	ofPopMatrix();

	cam.end();
	// ofDisableLighting();
	ofDisableDepthTest();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (tolower(key)) {
	 	case ' ': {
			playRandom();
			break;
		}
		// case 'r':
		// 	if (ma_sound_is_playing(soundscapes[currentSoundscape]->ambientSound)) {
		// 		cout << "stop" << endl;
		// 		ms_soundscape_stop(soundscapes[currentSoundscape]);
		// 	} else {
		// 		cout << "start" << endl;
		// 		ms_soundscape_start(soundscapes[currentSoundscape]);
		// 	}
		// 	if (ma_sound_get_current_fade_volume(soundscapes[currentSoundscape]->ambientSound) < 0.01) {
		// 		cout << "start" << endl;
		// 		ms_soundscape_start(soundscapes[currentSoundscape]);
		// 	}
		// 	break;
		case 'e':
			ms_soundscape_stop(soundscapes[currentSoundscape]);
			currentSoundscape++;
			if (currentSoundscape > 2) {
				currentSoundscape = 0;
			}
			ms_soundscape_start(soundscapes[currentSoundscape]);
			break;
		case 'q':
			ms_soundscape_stop(soundscapes[currentSoundscape]);
			currentSoundscape--;
			if (currentSoundscape < 0) {
				currentSoundscape = 2;
			}
			ms_soundscape_start(soundscapes[currentSoundscape]);
			break;
		case 'w':
			user.pos.y--;
			if (user.pos.y < 0) user.pos.y = TILE_WIDTH - 1;
			updateSoundscape();
			break;
		case 's':
			user.pos.y++;
			if (user.pos.y > TILE_WIDTH - 1) user.pos.y = 0;
			updateSoundscape();
			break;
		case 'a':
			user.pos.x--;
			if (user.pos.x < 0) user.pos.x = TILE_WIDTH - 1;
			updateSoundscape();
			break;
		case 'd':
			user.pos.x++;
			if (user.pos.x > TILE_WIDTH - 1) user.pos.x = 0;
			updateSoundscape();
			break;
	}
}
void ofApp::playRandom() {
	timeSinceLastPlayed = 0;
	ms_soundbite* sb = ms_soundscape_get_random_soundbite(soundscapes[currentSoundscape]);
	ms_soundbite_set_pan(sb, ofRandom(2) - 1);
	ms_soundbite_start(sb);
}

void ofApp::updateSoundscape() {
	if (user.pos.y == 1 && user.pos.x == 1) {
		ms_soundscape_stop(soundscapes[currentSoundscape]);
		currentSoundscape = 0;
		ms_soundscape_start(soundscapes[currentSoundscape]);
	} else if (user.pos.y == 1 && user.pos.x == 0) {
		ms_soundscape_stop(soundscapes[currentSoundscape]);
		currentSoundscape = 1;
		ms_soundscape_start(soundscapes[currentSoundscape]);
	} else if (user.pos.y == 0 && user.pos.x == 1) {
		ms_soundscape_stop(soundscapes[currentSoundscape]);
		currentSoundscape = 2;
		ms_soundscape_start(soundscapes[currentSoundscape]);
	} else if (user.pos.y == 0 && user.pos.x == 0) {
		ms_soundscape_stop(soundscapes[currentSoundscape]);
		currentSoundscape = 3;
		ms_soundscape_start(soundscapes[currentSoundscape]);
	}
}