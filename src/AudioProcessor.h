#pragma once

#include <iostream>
#include <pulse/pulseaudio.h>

class AudioProcessor
{
protected:
    /* data */

    u_int m_sampleRate;
    u_char m_sampleSize;
    u_char m_channels;
    u_int m_bufferMaxSize;

    bool m_initialised = false;

    pa_sample_spec m_spec;
public:
    AudioProcessor(/* args */);
    ~AudioProcessor();

    void init(u_int sampleRate, u_char sampleSize, u_char channels, u_int bufferLength_msec);

    virtual void process(short int * bufferPtr, size_t num) = 0;
};
