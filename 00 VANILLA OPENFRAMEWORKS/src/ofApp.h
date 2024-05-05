#pragma once

#include <stdarg.h>
#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
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

		double deltaTime = 0;

		struct Sound {
			string filepath;
			vector<ofSoundPlayer> samples;
			float volume;
			float timeSinceLastPlayed;
			float timeInterval;

			void init(string filepath, float vol = 1.0, int fileAmt = 1, bool loop = false) {
				this->filepath = filepath;
				this->volume = vol;

				if (fileAmt > 1) {
					cout << "Loading " << filepath << "[0-" << fileAmt - 1 << "].wav" << endl;
				} else {
					cout << "Loading " << filepath << ".wav" << endl;
				}

				for (int i = 0; i < fileAmt; i++) {
					string str = to_string(i);
					if (fileAmt == 1) str = "";

					ofSoundPlayer s;
					s.load(filepath + str + ".wav");
					s.setLoop(loop);
					s.setVolume(vol);
					samples.push_back(s);
				}

				cout << "Loaded " << fileAmt << " file(s)" << endl;
			}

			ofSoundPlayer getSample() { return samples[(int)ofRandom(samples.size())]; }
			string getPlaying() {
				string out = "";
				for (size_t i = 0; i < samples.size(); i++) {
					out += (samples[i].isPlaying()) ? this->filepath + to_string(i) + ".wav\n" : "";
				}
				return out;
			}

			void play() { ofSoundPlayer s = getSample(); s.setVolume(this->volume); cout << this->volume << endl; s.play(); };
		};

		struct Mood {			      // a mood is like a soundscape preset
			string name;
			Sound ambientSound;       // ambient sounds loop for the entire duration of the soundscape
			vector<Sound> soundbites; // soundbites are small sounds that are dispersed at random times

			void init(string name, Sound ambsnd, int numOfBites, ...) {
				this->name = name;
				this->ambientSound = ambsnd;

				va_list vl;
				va_start(vl, numOfBites);

				for (int i = 0; i < numOfBites; i++) {
					this->soundbites.push_back(va_arg(vl, Sound));
				}
				cout << this->name << " :: " << numOfBites << " soundbites" << endl;

				va_end(vl);
			}

			ofSoundPlayer getRandomSoundbite() { return soundbites[(int)ofRandom(soundbites.size())].getSample(); };
			Sound getRandomSound() { return soundbites[(int)ofRandom(soundbites.size())]; };
			ofSoundPlayer getAmbientSound() { return ambientSound.getSample(); }
			string getPlaying() {
				string out = "";
				out += ambientSound.filepath + "\n";
				for (size_t i = 0; i < soundbites.size(); i++) {
					out += soundbites[i].getPlaying();
				}
				return out;
			}

			// void fadeOut(float length) {
			// 	float normalvol = ambientSound.volume; // this is the volume that the ambsnd is normally at
			// 	double dt = 0;
			// 	cout << getAmbientSound().getVolume() << endl;

			// 	float v = 1.0;
			// 	while (v > 0.0) {
			// 		// v = v / length;
			// 		float x = ofMap(dt, 0, length, 0, 1);
			// 		v *= -x;
			// 		cout << v << endl;
			// 	}
			// }
		};

		Sound ambRain, ambSunny, ambCityHum;
		Sound sbCarDriving, sbThunder, sbMultiTest, sbJardins;
		vector<Sound> soundbites;

		Mood mRainy, mSunny, mUrban;

		vector<Mood> moods;

		unsigned char currentMood;
};
