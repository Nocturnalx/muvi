#pragma once

#include <iostream>
#include <math.h>
#include "AudioProcessor.h"
#include "../kissfft/kiss_fftr.h"

#ifndef NFFT
    #define NFFT 1024
#endif
#ifndef BINS
    #define BINS 513
#endif

class FFTProcessor : public AudioProcessor
{
private:
    /* data */
    u_int m_nfft = NFFT;

    kiss_fftr_cfg m_cfg = kiss_fftr_alloc(m_nfft, 0, NULL, NULL);

    kiss_fft_scalar * m_inputBuffer;
    kiss_fft_cpx * m_cpxBuffer;

    float * m_magnitudeBuffer; //pointer to external magnitude buffer array (so points to stack array no need to delete [])

    u_int m_inputBufferIndex = 0;

    float m_normalisationValue = logf(1+1024);
    
public:
    
    FFTProcessor();
    ~FFTProcessor();

    bool frameReady = false;

    void process(short int * bufferPtr, size_t num) override;

    void bindMagnitudeBuffer(float * magnitudeBuffer);
};