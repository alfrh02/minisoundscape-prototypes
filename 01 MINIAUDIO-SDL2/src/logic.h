#ifndef LOGIC_H
#define LOGIC_H

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {
	if (fabs(inputMin - inputMax) < std::numeric_limits<float>::epsilon()) {
		return outputMin;
	} else {
		return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
	}
}

static float adsr(float attack, float decay, float sustain, float release, float time) {
    std::string state;
    float out = 0.0;

    if (time < attack) {
        state = "attack";
        out = map(time, 0.0, attack, 0.0, 1.0);
    } else if (time < attack + decay) {
        state = "decay";
        out = map(time, attack, decay, 1.0, sustain);
    } else if (time > 1.0) {
        state = "release";
        out = map(time, 1.0, release, sustain, 0.0);
    } else {
        state = "sustain";
        out = sustain;
    }

    if (out < 0) {
        // printf("adsr() ERROR: returning below 0 - %i\n", out);
        out = 0;
    } else if (out > 1) {
        printf("adsr() ERROR: returning above 1 - %i\n", out);
        out = 1;
    }

    #ifndef __EMSCRIPTEN__
        std::cout << "t: " << time << "\nout: " << out << ", " << state << std::endl;
    #endif

    return out;
}

#endif