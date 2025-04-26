#ifndef __stdcall
#define __stdcall
#endif

#ifndef _WIN32
#define __stdcall
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "lime/LimeSuite.h"
#include "TSDRPlugin.h"

#define TSDR_OK 0
#define TSDR_PLUGIN_HARDWARE_MISSING -1
#define TSDR_PLUGIN_HARDWARE_ERROR -2
#define TSDR_PLUGIN_OTHER -3

#define BUFFER_SIZE 8192

static lms_device_t* device = NULL;
static lms_stream_t stream;
static int streaming = 0;
static float *float_buffer = NULL;

// Valeurs configurables (en Hertz)
static uint32_t current_samplerate = 8000000;
static uint32_t current_frequency = 284000000;  
static char last_error[256] = "";

int TSDRPLUGIN_API __stdcall tsdrplugin_init(const char * params) {
    if (params != NULL) {
        sscanf(params, "%u %u", &current_frequency, &current_samplerate);
    }

    if (LMS_GetDeviceList(NULL) < 1) {
        snprintf(last_error, sizeof(last_error), "No LimeSDR device found.");
        return TSDR_PLUGIN_HARDWARE_MISSING;
    }

    if (LMS_Open(&device, NULL, NULL)) {
        snprintf(last_error, sizeof(last_error), "Failed to open LimeSDR device.");
        return TSDR_PLUGIN_HARDWARE_MISSING;
    }

    if (LMS_Init(device) != 0) {
        snprintf(last_error, sizeof(last_error), "Failed to initialize LimeSDR device.");
        return TSDR_PLUGIN_HARDWARE_ERROR;
    }

    LMS_EnableChannel(device, LMS_CH_RX, 0, true);
    LMS_SetSampleRate(device, current_samplerate, 2);
    LMS_SetLOFrequency(device, LMS_CH_RX, 0, current_frequency);
    LMS_SetGaindB(device, LMS_CH_RX, 0, 50); // Gain moyen

    memset(&stream, 0, sizeof(lms_stream_t));
    stream.channel = 0;
    stream.fifoSize = 1 * 1024 * 1024;
    stream.throughputVsLatency = 1.0;
    stream.isTx = false;
    stream.dataFmt = LMS_FMT_I16;

    if (LMS_SetupStream(device, &stream) != 0) {
        snprintf(last_error, sizeof(last_error), "Failed to set up LimeSDR stream.");
        return TSDR_PLUGIN_HARDWARE_ERROR;
    }

    float_buffer = (float *)malloc(sizeof(float) * BUFFER_SIZE * 2);
    if (!float_buffer) {
        snprintf(last_error, sizeof(last_error), "Failed to allocate float buffer.");
        return TSDR_PLUGIN_OTHER;
    }

    return TSDR_OK;
}

uint32_t TSDRPLUGIN_API __stdcall tsdrplugin_setsamplerate(uint32_t rate) {
    current_samplerate = rate;
    LMS_SetSampleRate(device, current_samplerate, 2);
    return current_samplerate;
}

uint32_t TSDRPLUGIN_API __stdcall tsdrplugin_getsamplerate() {
    return current_samplerate;
}

int TSDRPLUGIN_API __stdcall tsdrplugin_setbasefreq(uint32_t freq) {
    current_frequency = freq;
    LMS_SetLOFrequency(device, LMS_CH_RX, 0, current_frequency);
    return TSDR_OK;
}

int TSDRPLUGIN_API __stdcall tsdrplugin_setgain(float gain) {
    LMS_SetGaindB(device, LMS_CH_RX, 0, gain);
    return TSDR_OK;
}

int TSDRPLUGIN_API __stdcall tsdrplugin_stop(void) {
    streaming = 0;
    return TSDR_OK;
}

char * TSDRPLUGIN_API __stdcall tsdrplugin_getlasterrortext(void) {
    return last_error;
}

int TSDRPLUGIN_API __stdcall tsdrplugin_readasync(tsdrplugin_readasync_function cb, void *ctx) {
    int16_t iq_buffer[BUFFER_SIZE * 2];
    streaming = 1;

    LMS_StartStream(&stream);

    while (streaming) {
        int samples_read = LMS_RecvStream(&stream, iq_buffer, BUFFER_SIZE, NULL, 1000);
        if (samples_read < 0) {
            snprintf(last_error, sizeof(last_error), "Stream read error.");
            break;
        }

        for (int i = 0; i < samples_read * 2; i++) {
            float_buffer[i] = iq_buffer[i] / 32768.0f;
        }

        cb(float_buffer, samples_read * 2, ctx, 0);
    }

    LMS_StopStream(&stream);
    return TSDR_OK;
}

void TSDRPLUGIN_API __stdcall tsdrplugin_cleanup(void) {
    if (device) {
        LMS_StopStream(&stream);
        LMS_DestroyStream(device, &stream);
        LMS_EnableChannel(device, LMS_CH_RX, 0, false);
        LMS_Close(device);
        device = NULL;
    }

    if (float_buffer) {
        free(float_buffer);
        float_buffer = NULL;
    }
}

void TSDRPLUGIN_API __stdcall tsdrplugin_getName(char * name) {
    strcpy(name, "TSDR LimeSDR Plugin");
}
                                                                                                                          
