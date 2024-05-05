#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);

    ambRain.init("ambient/rain", 0.8, 1, true);
    ambCityHum.init("ambient/city-hum", 0.65, 1, true);

    // filepath (without `.wav`!), volume = 1.0, number of files = 1.0
    // sbThunder.init("soundbites/thunder", 0.8, 2);
    sbThunder.init(ofToDataPath("soundbites/thunder", false), 0.8, 2);
    sbCarDriving.init("soundbites/car-driving", 0.25);
    sbMultiTest.init("soundbites/multi", 0.5, 3);
    sbJardins.init("soundbites/jardins", 0.25, 3);

    // mRainy.init("rainy", ambRain, 1, sbMultiTest);
    mRainy.init("rainy", ambRain,    2, sbThunder, sbCarDriving);
    // mSunny.init("sunny", ambRain, 1, sbMultiTest);
    mUrban.init("urban", ambCityHum, 1, sbJardins);

    currentMood = 0;

    moods.push_back(mRainy);
    // moods.push_back(mSunny);
    moods.push_back(mUrban);

    moods[currentMood].getAmbientSound().play();
}

//--------------------------------------------------------------
void ofApp::update(){
    deltaTime += ofGetLastFrameTime();
}

//--------------------------------------------------------------
void ofApp::draw(){
    stringstream s;
    s << "Change mood with A or D" << endl;
    s << "Mood: (" + to_string(currentMood + 1) + ") " + moods[currentMood].name << endl;
    s << "---" << endl;
    s << moods[currentMood].getPlaying() << endl;

    ofDrawBitmapString(s.str().c_str(), 8, 12);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (tolower(key)) {
        case 'a':
            moods[currentMood].getAmbientSound().stop();
            if (currentMood - 1 < 0) {
                currentMood = moods.size() - 1;
            } else {
                currentMood--;
            }
            moods[currentMood].getAmbientSound().play();
            break;
        case 'd':
            moods[currentMood].getAmbientSound().stop();
            if (currentMood + 1 >= moods.size()) {
                currentMood = 0;
            } else {
                currentMood++;
            }
            moods[currentMood].getAmbientSound().play();
            break;
        case 'w':
            break;
        case 's':
            // moods[currentMood].fadeOut(2);
            // moods[currentMood].getAmbientSound().setVolume(0);
            break;
        case ' ':
            Sound s = moods[currentMood].getRandomSound();
            if (s.getPlaying() == "") {
                ofSoundPlayer sp = s.getSample();
                sp.play();
                sp.setPan(ofRandom(2) - 1);
            }
    }
}