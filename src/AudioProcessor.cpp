#include "AudioProcessor.h"

AudioProcessor::AudioProcessor()
{
}

AudioProcessor::~AudioProcessor()
{
}

void AudioProcessor::init(u_int sampleRate, u_char sampleSize, u_char channels, u_int bufferLength_msec){

    m_sampleRate = sampleRate;
    m_sampleSize = sampleSize;
    m_channels = channels;
    m_bufferMaxSize = sampleRate * sampleSize * channels * bufferLength_msec / 1000;

    m_initialised = true;
}