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

void FFTProcessor::bindDisplayBuffer(float * displayBuffer){

    m_displayBuffer = displayBuffer;
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

            int width = 1;
            float intermediate = 1;
            int roller = 64;
            int rollerIndex = 0;

            int cpxIndex = 0;
            int displayBufferIndex = 0;

            //here we go up to 512 only because it makes the maths easier 
            //and we're not too bothered about UHFs
            //one loop of this is one value in display buff
            while(cpxIndex + width < BINS){
                
                float val = 0;
                float valMax = 0;

                //get largest value from bin collection
                for (int i = 0; i < width; i++){

                    val = m_cpxBuffer[cpxIndex].r * m_cpxBuffer[cpxIndex].r + m_cpxBuffer[cpxIndex].i * m_cpxBuffer[cpxIndex].i;

                    if (val > valMax){
                        valMax = val;
                    }

                    cpxIndex++;
                }

                //normalise bbiggest val
                valMax = logf(1 + valMax);
                valMax /= m_normalisationValue;

                //clip 0-1
                if (valMax > 1.0f) valMax = 1.0f;
                if (valMax < 0.0f) valMax = 0.0f;

                //add to buffer
                m_displayBuffer[displayBufferIndex] = valMax; 

                // intermediate *= 1.06;

                if (++rollerIndex % roller == 0){
                    // width = ceil(intermediate);
                    width *= 4;
                    roller /= 2;
                }

                displayBufferIndex++;
            }

            frameReady = true;

            // ##linear magnitude values##

            //calculate amplitude values as float 0-1
            // for (int i = 0; i < BINS; i++){

            //     float val;

            //     val = m_cpxBuffer[i].r * m_cpxBuffer[i].r + m_cpxBuffer[i].i * m_cpxBuffer[i].i;
            //     val = logf(1 + val);
            //     val /= m_normalisationValue;

            //     if (val > 1.0f) val = 1.0f;
            //     if (val < 0.0f) val = 0.0f;

            //     m_magnitudeBuffer[i] = val;
            // }
        }
    }
}