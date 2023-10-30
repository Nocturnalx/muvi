#pragma once

#include <string.h>
#include <iostream>
#include <memory>

#include "FFTProcessor.h"

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 48000
#endif

#ifndef CHANNEL_NUM
#define CHANNEL_NUM 1
#endif

#ifndef BUFFER_LENGTH_MSEC
#define BUFFER_LENGTH_MSEC 500
#endif

static int inputDevice = 1;
static int outputDevice = 0;

static int inputDeviceIndex = 0;
static int outputDeviceIndex = 0;
static std::string inputDeviceID;
static std::string outputDeviceID;

static pa_mainloop_api * mlapi; //get api
static pa_context * ctx; //get context with application name and pr

extern bool audioRunning;
extern bool threadComplete;

class AudioManager {
private:
    /* data */
    float m_magnitudeData[BINS];

public:
    AudioManager();
    ~AudioManager();

    std::unique_ptr<FFTProcessor> processor;

    float * getMagnitudeData();
};

//PA callback functions

//context state change
static void on_state_change(pa_context *c, void *userdata);
//gather output devices
static void on_dev_sink(pa_context * c, const pa_sink_info *info, int eol, void * udata);
//gather input devices
static void on_dev_source(pa_context *c, const pa_source_info *info, int eol, void *udata);
//do and unreference pa operation
static void do_op(pa_operation * op);
//callback for when new data is available in the stream
static void on_io_complete(pa_stream *s, size_t nbytes, void *udata);

//gets list of device IDs as well as setting desired in/out IDs for PA
static void getDeviceIDs();
//start the audio recording/processing
static void start(pa_context *ctx, std::shared_ptr<AudioManager> audioManager);

void setupAudio(std::shared_ptr<AudioManager> audioManager);