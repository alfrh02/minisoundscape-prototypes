#ifndef MINISOUNDSCAPE_H
#define MINISOUNDSCAPE_H

#include "miniaudio.h"

#include <stdarg.h> // variadic arguments
#include <vector>

#define FADE_AMOUNT_IN_MS 1000

/*

    minisoundscape is an addon for miniaudio that adds utilities
    primarily for game development regarding procedural sounds.

    a soundscape is simply a collection of sounds. soundscapes contain:
    1. a looping ambient sound that typically defines the undercurrent
       "theme" that may be intended
    2. a group of "soundbites", one of which may contain multiple sounds,
       that are played at random points

    TODO:
    1. currently soundbites are played manually - add a system that can
       play soundbites independently while taking different weights into
       account (some soundbites may "weigh" more and therefore be more
       common).
    2. add spatialised sounds that may move as the in-game player is
       moved.

    You can view the source code below. View the ofApp.cpp tab for a small
    implementation example.

*/

// const ma_sound* MS_NULL_SOUND = nullptr;

/* --- ms_soundbite --- */

struct ms_soundbite {
    std::string name;
    vector<ma_sound*> samples
    int weight;
    float panning_width;
};

ma_result ms_soundbite_init(ma_engine* engine, ms_soundbite* soundbite, const std::string filepath, const int fileAmt = 1);
// ma_result ms_soundbite_init(ma_engine* engine, ms_soundbite* soundbite, const ma_sound* sound);
ma_result ms_soundbite_uninit(ms_soundbite* soundbite);
ma_result ms_soundbite_start(const ms_soundbite* soundbite);
bool ms_soundbite_is_playing(const ms_soundbite* soundbite);
ma_result ms_soundbite_add_sample(ms_soundbite* soundbite, ma_sound* sound);
ma_result ms_soundbite_add_sample(ma_engine* engine, ms_soundbite* soundbite, const std::string filepath, const int fileAmt = 1);
ma_result ms_soundbite_set_pan(const ms_soundbite* soundbite, float pan);
ma_result ms_soundbite_set_volume(const ms_soundbite* soundbite, float volume);

// init/uninit

ma_result ms_soundbite_init(ma_engine* engine, ms_soundbite* soundbite, const std::string filepath, const int fileAmt) {
    soundbite->name = filepath;
    #ifdef MS_VERBOSE
        cout << "ms_soundbite_init :: initialising " << soundbite->name << endl;
    #endif
    return ms_soundbite_add_sample(engine, soundbite, filepath, fileAmt);
}

// ma_result ms_soundbite_init(ms_soundbite* soundbite, const ma_sound* sound) {
//     soundbite->name = "NULL";
//     #ifdef MS_VERBOSE
//         cout << "ms_soundbite_init :: loading " << soundbite->name << endl;
//     #endif
//     return ms_soundbite_add_sample(soundbite, sound);
// }

ma_result ms_soundbite_uninit(ms_soundbite* soundbite) {
    for (ma_sound* s : soundbite->samples) {
        delete s;
    }
    delete soundbite;
    return MA_SUCCESS;
}

// mechanics

ma_result ms_soundbite_start(const ms_soundbite* soundbite) {
    #ifdef MS_VERBOSE
        std::cout << "ms_soundbite_start :: playing " << soundbite->name << endl;
    #endif
    if (ms_soundbite_is_playing(soundbite)) return MA_SUCCESS;
    ma_result result = ma_sound_start(soundbite->samples[rand() % soundbite->samples.size()]);
    return result;
}

// getters

bool ms_soundbite_is_playing(const ms_soundbite* soundbite) {
    for (ma_sound* s : soundbite->samples) {
        if (ma_sound_is_playing(s)) return true;
    }
    return false;
}

// setters

ma_result ms_soundbite_add_sample(ms_soundbite* soundbite, ma_sound* sound) {
    soundbite->samples.push_back(sound);
    return MA_SUCCESS;
}

ma_result ms_soundbite_add_sample(ma_engine* engine, ms_soundbite* soundbite, const std::string filepath, const int fileAmt) {
    #ifdef MS_VERBOSE
        cout << "ms_soundbite_add_sample :: loading " << soundbite->name << " with " << fileAmt << " files" << endl;
    #endif
    for (int i = 0; i < fileAmt; i++) {
        std::string str = filepath;
        if (fileAmt > 1) {
            str += to_string(i) + ".wav";
            #ifdef MS_VERBOSE
                cout << "ms_soundbite_add_sample :: " << to_string(i + 1) << ": " << str << endl;
            #endif
        }

        // allocate on the heap as to not invalidate when we go out of scope after pushing into vector
        ma_sound* s = new ma_sound;
        ma_result result = ma_sound_init_from_file(engine, str.c_str(), 0, NULL, NULL, s);
        if (result != MA_SUCCESS) return result;
        ms_soundbite_add_sample(soundbite, s);
    }

    return MA_SUCCESS;
}

ma_result ms_soundbite_set_pan(const ms_soundbite* soundbite, float pan) {
    for (ma_sound* sb : soundbite->samples) {
        ma_sound_set_pan(sb, pan);
    }
    return MA_SUCCESS;
}

ma_result ms_soundbite_set_volume(const ms_soundbite* soundbite, float volume) {
    for (ma_sound* sb : soundbite->samples) {
        ma_sound_set_volume(sb, volume);
    }
    return MA_SUCCESS;
}

/* --- ms_soundscape --- */

struct ms_soundscape {
    std::string name;
    ma_sound* ambientSound;
    vector<ms_soundbite*> soundbites;
};

ma_result ms_soundscape_init(ms_soundscape* soundscape, const std::string name, const ma_sound* ambsnd, const int soundbiteAmt, ...);
ma_result ms_soundscape_uninit(ms_soundscape* soundscape);
ma_result ms_soundscape_start(const ms_soundscape* soundscape);
// ma_result ms_soundscape_tick(const ms_soundscape* soundscape);
ma_result ms_soundscape_add_soundbite(ms_soundscape* soundscape, ms_soundbite* soundbite);
// ma_result ms_soundscape_add_soundbite(ms_soundscape* soundscape, ma_sound* sound);
ms_soundbite* ms_soundscape_get_random_soundbite(const ms_soundscape* soundscape);

// init/uninit

ma_result ms_soundscape_init(ms_soundscape* soundscape, const std::string name, ma_sound* ambsnd, const int soundbiteAmt, ...) {
    soundscape->name = name;

    soundscape->ambientSound = ambsnd;
    if (soundscape->ambientSound != nullptr) ma_sound_set_looping(soundscape->ambientSound, true);

    va_list vl;
    va_start(vl, soundbiteAmt);

    for (int i = 0; i < soundbiteAmt; i++) {
        ma_result result = ms_soundscape_add_soundbite(soundscape, va_arg(vl, ms_soundbite*));
        if (result != MA_SUCCESS) return result;
    }

    va_end(vl);

    #ifdef MS_VERBOSE
        cout << name << " :: " << soundbiteAmt << " soundbites loaded" << endl;
    #endif

    return MA_SUCCESS;
}

ma_result ms_soundscape_uninit(ms_soundscape* soundscape) {
    for (size_t i = 0; i < soundscape->soundbites.size(); i++) {
        ms_soundbite_uninit(soundscape->soundbites[i]);
        i--;
    }
    delete soundscape->ambientSound;
    delete soundscape;
    return MA_SUCCESS;
}

// mechanics

// https://github.com/mackron/miniaudio/issues/714
ma_result ms_soundscape_start(const ms_soundscape* soundscape) {
    // ma_sound_set_volume(soundscape->ambientSound, 1.0);
    ma_sound_set_stop_time_in_milliseconds(soundscape->ambientSound, ~(ma_uint64)0);
    ma_sound_set_fade_in_milliseconds(soundscape->ambientSound, 0.0f, 1.0f, FADE_AMOUNT_IN_MS);
    // ma_sound_seek_to_pcm_frame(soundscape->ambientSound, 0);
    if (soundscape->ambientSound != nullptr) ma_sound_start(soundscape->ambientSound);
    return MA_SUCCESS;
}

ma_result ms_soundscape_stop(const ms_soundscape* soundscape) {
    // ma_sound_set_stop_time_in_milliseconds(soundscape->ambientSound, FADE_AMOUNT_IN_MS + 1000);
    ma_sound_set_fade_in_milliseconds(soundscape->ambientSound, -1.0f, 0.0f, FADE_AMOUNT_IN_MS);
    return MA_SUCCESS;
}

// getters

ms_soundbite* ms_soundscape_get_random_soundbite(const ms_soundscape* soundscape) {
    return soundscape->soundbites[rand() % soundscape->soundbites.size()];
}

// setters

ma_result ms_soundscape_add_soundbite(ms_soundscape* soundscape, ms_soundbite* soundbite) {
    soundscape->soundbites.push_back(soundbite);
    return MA_SUCCESS;
}

#endif // MINISOUNDSCAPE_H