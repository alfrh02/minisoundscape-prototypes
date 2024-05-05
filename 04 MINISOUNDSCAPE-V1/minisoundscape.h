#ifndef MINISOUNDSCAPE_H
#define MINISOUNDSCAPE_H

#include "miniaudio.h"

#include <stdarg.h> // variadic arguments
#include <vector>

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

#define FADE_AMOUNT_IN_MS 1000

/* --- minisoundscape --- */

// static ma_engine* ms_engine;

// ma_result ms_set_engine(ma_engine* engine);

// ma_result ms_set_engine(ma_engine* engine) {
//     ms_engine = engine;
// }

// /* --- ms_soundbite --- */

struct ms_soundbite {
    std::string name;
    ma_engine* engine;
    vector<ma_sound*> samples;
    int weight;
};

ma_result ms_soundbite_init(ma_engine* engine, ms_soundbite* soundbite, const std::string filepath, const int fileAmount = 1);
ma_result ms_soundbite_uninit(ms_soundbite* soundbite);
ma_result ms_soundbite_start(const ms_soundbite* soundbite);
bool      ms_soundbite_is_playing(const ms_soundbite* soundbite);
ma_result ms_soundbite_add_sample(ms_soundbite* soundbite, ma_sound* sound);
ma_result ms_soundbite_add_sample(ms_soundbite* soundbite, const std::string filepath, const int fileAmount = 1);
// ma_result ms_soundbite_set_pan(const ms_soundbite* soundbite, float pan);
// ma_result ms_soundbite_set_volume(const ms_soundbite* soundbite, float volume);

ma_result ms_soundbite_init(ma_engine* engine, ms_soundbite* soundbite, const std::string filepath, const int fileAmount) {
    soundbite->name = filepath;
    soundbite->engine = engine;
    #ifdef MS_VERBOSE
        std::cout << "ms_soundbite_init :: initialising " << soundbite->name << std::endl;
    #endif
    return ms_soundbite_add_sample(soundbite, filepath, fileAmount);
}

ma_result ms_soundbite_uninit(ms_soundbite* soundbite) {
    for (ma_sound* s : soundbite->samples) {
        delete s;
    }
    delete soundbite;
    return MA_SUCCESS;
}

ma_result ms_soundbite_start(const ms_soundbite* soundbite) {
    // check if the soundbite is already playing; if it is, we return early
    if (ms_soundbite_is_playing(soundbite)) return MA_SUCCESS;
    // else start the soundbite, choosing a random sample

    int index = rand() % soundbite->samples.size();
    #ifdef MS_VERBOSE
        if (soundbite->samples.size() > 1) std::cout << "ms_soundbite_start :: playing " << soundbite->name << index << std::endl;
        else                               std::cout << "ms_soundbite_start :: playing " << soundbite->name << std::endl;
    #endif

    return ma_sound_start(soundbite->samples[0]);
}

bool ms_soundbite_is_playing(const ms_soundbite* soundbite) {
    for (ma_sound* s : soundbite->samples) {
        if (ma_sound_is_playing(s)) return true;
    }
    return false;
}

ma_result ms_soundbite_add_sample(ms_soundbite* soundbite, ma_sound* sound) {
    #ifdef MS_VERBOSE
        std::cout << "ms_soundbite_add_sample :: loading " << soundbite->name << " with 1 file" << std::endl;
    #endif
    soundbite->samples.push_back(sound);
    return MA_SUCCESS;
}

ma_result ms_soundbite_add_sample(ms_soundbite* soundbite, const std::string filepath, const int fileAmount) {
    if (soundbite->engine == nullptr) {
        #ifdef MS_VERBOSE
            std::cout << "ms_soundbite_add_sample :: soundbite " << soundbite->name << " has no engine" << std::endl;
        #endif
        return MA_ERROR;
    }
    #ifdef MS_VERBOSE
        std::cout << "ms_soundbite_add_sample :: loading " << soundbite->name << " with " << fileAmount << " files" << std::endl;
    #endif
    for (size_t i = 0; i < fileAmount; i++) {
        std::string str = filepath;

        // calculate filepath
        if (fileAmount > 1) {
            str += to_string(i) + ".wav";
            #ifdef MS_VERBOSE
                std::cout << "ms_soundbite_add_sample :: loading " << to_string(i + 1) << ": " << str << std::endl;
            #endif
        } else {
            str += ".wav";
        }

        // allocate file to a new ma_sound pointer
        ma_sound* s = new ma_sound;
        ma_result result = ma_sound_init_from_file(soundbite->engine, str.c_str(), 0, NULL, NULL, s);

        // if we fail to allocate the file, return early
        if (result != MA_SUCCESS) return result;

        // else push it to `soundbite`'s samples vector
        ms_soundbite_add_sample(soundbite, s);
    }
    return MA_SUCCESS;
}

/* --- ms_soundscape --- */

struct ms_soundscape {
    std::string name;
    ma_sound* ambient;
    vector<ms_soundbite*> soundbites;
};

ma_result ms_soundscape_init(ms_soundscape* soundscape, const std::string name, ma_sound* ambient, const int soundbiteAmount, ...);
ma_result ms_soundscape_uninit(ms_soundscape* soundscape);
ma_result ms_soundscape_start(const ms_soundscape* soundscape);
ma_result ms_soundscape_add_soundbite(ms_soundscape* soundscape, ms_soundbite* soundbite);
ms_soundbite* ms_soundscape_get_random_soundbite(const ms_soundscape* soundscape);

ma_result ms_soundscape_init(ms_soundscape* soundscape, const std::string name, ma_sound* ambient, const int soundbiteAmount, ...) {
    soundscape->name = name;
    soundscape->ambient = ambient;
    if (soundscape->ambient != nullptr) ma_sound_set_looping(ambient, true);

    va_list vl;
    va_start(vl, soundbiteAmount);

    for (int i = 0; i < soundbiteAmount; i++) {
        ma_result result = ms_soundscape_add_soundbite(soundscape, va_arg(vl, ms_soundbite*));
        if (result != MA_SUCCESS) return result;
    }

    va_end(vl);

    #ifdef MS_VERBOSE
        std::cout << name << " :: " << soundbiteAmount << " soundbite(s) loaded" << std::endl;
    #endif

    return MA_SUCCESS;
}

ma_result ms_soundscape_uninit(ms_soundscape* soundscape) {
    for (size_t i = 0; i < soundscape->soundbites.size(); i++) {
        ms_soundbite_uninit(soundscape->soundbites[i]);
        i--;
    }
    delete soundscape->ambient;
    delete soundscape;
    return MA_SUCCESS;
}

// https://github.com/mackron/miniaudio/issues/714
ma_result ms_soundscape_start(const ms_soundscape* soundscape) {
    if (soundscape->ambient != nullptr) {
        ma_sound_set_stop_time_in_milliseconds(soundscape->ambient, ~(ma_uint64)0);
        ma_sound_set_fade_in_milliseconds(soundscape->ambient, 0.0f, 1.0f, FADE_AMOUNT_IN_MS);
        ma_sound_start(soundscape->ambient);
    }
    return MA_SUCCESS;
}

ma_result ms_soundscape_stop(const ms_soundscape* soundscape) {
    ma_sound_set_fade_in_milliseconds(soundscape->ambient, -1.0f, 0.0f, FADE_AMOUNT_IN_MS);
    return MA_SUCCESS;
}

ms_soundbite* ms_soundscape_get_random_soundbite(const ms_soundscape* soundscape) {
    return soundscape->soundbites[rand() % soundscape->soundbites.size()];
}

ma_result ms_soundscape_add_soundbite(ms_soundscape* soundscape, ms_soundbite* soundbite) {
    soundscape->soundbites.push_back(soundbite);
    return MA_SUCCESS;
}

#endif // MINISOUNDSCAPE_H