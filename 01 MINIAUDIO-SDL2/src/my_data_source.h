#include "../libs/miniaudio.h"
#include "logic.h"

#define SAMPLE_RATE 44100
#define PI          3.14159265358979323846

struct my_data_source
{
    ma_data_source_base base;
    float frequency;
    ma_uint64 length;
    double time, advance;
    float attack, decay, sustain, release;
};

static ma_result my_data_source_read(ma_data_source* pDataSource, void* pFramesOut, ma_uint64 frameCount, ma_uint64* pFramesRead)
{
    // Read data here. Output in the same format returned by my_data_source_get_data_format().
    printf("reading %i frames\n", frameCount);

    my_data_source* ds = (my_data_source*)pDataSource;

    if (ds->time > ds->length) {
        printf("ok\n");
    }

    if (pFramesRead != NULL) {
        *pFramesRead = 0;
    }

    if (frameCount == 0) {
        return MA_INVALID_ARGS;
    }

    if (pDataSource == NULL) {
        return MA_INVALID_ARGS;
    }

    if (pFramesOut == NULL) {
        return MA_INVALID_ARGS;
    } else {
        float* pFramesOutF32 = (float*)pFramesOut;
        for (ma_uint64 i = 0; i < frameCount; i += 1) {
            float a = adsr(ds->attack, ds->decay, ds->sustain, ds->release, ds->time / ds->length) * 0.5;

            float s = sin(2 * PI * ds->frequency * ds->time) * a; // sin(2 * pi * f * t) * a

            ds->time += ds->advance;               // advance by 1 sample

            for (ma_uint64 c = 0; c < 2; c += 1) { // channels!
                pFramesOutF32[i*2 + c] = s;
            }
        }
    }

    if (pFramesRead != NULL) {
        *pFramesRead = frameCount;
    }

    return MA_SUCCESS;
}

static ma_result my_data_source_seek(ma_data_source* pDataSource, ma_uint64 frameIndex)
{
    // Seek to a specific PCM frame here. Return MA_NOT_IMPLEMENTED if seeking is not supported.
    printf("seeking @ frameIndex %i\n", frameIndex);

    my_data_source* ds = (my_data_source*)pDataSource;

    ds->time = frameIndex;

    return MA_NOT_IMPLEMENTED;
}

static ma_result my_data_source_get_data_format(ma_data_source* pDataSource, ma_format* pFormat, ma_uint32* pChannels, ma_uint32* pSampleRate, ma_channel* pChannelMap, size_t channelMapCap)
{
    printf("get data format\n");

    my_data_source* ds = (my_data_source*)pDataSource;

    if (pDataSource == NULL) {
        return MA_INVALID_ARGS;
    }

    if (pFormat != NULL) {
        *pFormat = ma_format_f32;
    }

    if (pChannels != NULL) {
        *pChannels = 2;
    }

    if (pSampleRate != NULL) {
        *pSampleRate = SAMPLE_RATE;
    }

    return MA_SUCCESS;
}

// static ma_result my_data_source_get_cursor(ma_data_source* pDataSource, ma_uint64* pCursor)
// {
//     // Retrieve the current position of the cursor here. Return MA_NOT_IMPLEMENTED and set *pCursor to 0 if there is no notion of a cursor.
//     printf("get cursor\n");
//     return MA_NOT_IMPLEMENTED;
// }

// static ma_result my_data_source_get_length(ma_data_source* pDataSource, ma_uint64* pLength)
// {
//     // Retrieve the length in PCM frames here. Return MA_NOT_IMPLEMENTED and set *pLength to 0 if there is no notion of a length or if the length is unknown.
//     printf("get length\n");
//     pLength = 0;
//     return MA_NOT_IMPLEMENTED;
// }

static ma_data_source_vtable g_my_data_source_vtable =
{
    my_data_source_read,
    my_data_source_seek,
    my_data_source_get_data_format//,
    // my_data_source_get_cursor,
    // my_data_source_get_length
};

ma_result my_data_source_init(my_data_source* pMyDataSource)
{
    printf("init\n");

    ma_result result;
    ma_data_source_config baseConfig;

    baseConfig = ma_data_source_config_init();
    baseConfig.vtable = &g_my_data_source_vtable;

    result = ma_data_source_init(&baseConfig, &pMyDataSource->base);
    if (result != MA_SUCCESS) {
        return result;
    }

    pMyDataSource->frequency  = 440;       // fundamental frequency

    pMyDataSource->time       = 0;
    pMyDataSource->advance    = 1.0 / (SAMPLE_RATE);
    pMyDataSource->length     = 1;

    pMyDataSource->attack     = 0.01;      // % of length
    pMyDataSource->decay      = 0.2;       // % of length
    pMyDataSource->sustain    = 0.8;       // amplitude constant
    pMyDataSource->release    = 2.0 + 1.0; // seconds + 1.0

    return MA_SUCCESS;
}

void my_data_source_uninit(my_data_source* pMyDataSource)
{
    printf("uninit\n");

    ma_data_source_uninit(&pMyDataSource->base);
}

#undef SAMPLE_RATE
#undef PI