#include "FFTProcessor.h"

FFTProcessor::FFTProcessor(){
    
    m_inputBuffer = new kiss_fft_scalar[m_nfft];
    m_cpxBuffer = new kiss_fft_cpx[m_nfft/2+1];
}

FFTProcessor::~FFTProcessor(){
    
    free(m_cfg);

    delete [] m_inputBuffer;
    delete [] m_cpxBuffer;
}

void FFTProcessor::bindMagnitudeBuffer(float * magnitudeBuffer){

    m_magnitudeBuffer = magnitudeBuffer;
}

void FFTProcessor::process(short int * bufferPtr, size_t num){

    if (!m_initialised) return; //dont allow processing if not initialised
    u_int sampleNumber = num / m_sampleSize; // /2 because we're using short int

    //process samples
    for (int i = 0; i < sampleNumber; i++){

        m_inputBuffer[m_inputBufferIndex] = (float)bufferPtr[i] / 32767.0;

        m_inputBufferIndex = (m_inputBufferIndex + 1) % m_nfft;

        if (m_inputBufferIndex == 0){

            frameReady = false;

            kiss_fftr(m_cfg, m_inputBuffer, m_cpxBuffer);

            //calculate amplitude values as float 0-1
            for (int i = 0; i < BINS; i++){

                float val;

                val = m_cpxBuffer[i].r * m_cpxBuffer[i].r + m_cpxBuffer[i].i * m_cpxBuffer[i].i;
                val = logf(1 + val);
                val /= m_normalisationValue;

                if (val > 1.0f) val = 1.0f;
                if (val < 0.0f) val = 0.0f;

                m_magnitudeBuffer[i] = val;
            }

            frameReady = true;
        }
    }
}