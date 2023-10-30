#include "AudioManager.h"

bool audioRunning = true;
bool threadComplete = false;

pa_threaded_mainloop * mloop;

//context state change
static void on_state_change(pa_context *c, void *userdata){
    pa_threaded_mainloop_signal(mloop, 0);
}
//gather output devices
static void on_dev_sink(pa_context * c, const pa_sink_info *info, int eol, void * udata){

    if (eol != 0){
        pa_threaded_mainloop_signal(mloop, 0);
        return; 
    }

    std::cout << "sink: " << outputDeviceIndex << ": " << info->name << '\n';

    if (outputDeviceIndex == outputDevice){

        outputDeviceID = info->name;

        outputDeviceIndex++;

        pa_threaded_mainloop_signal(mloop, 0);
        return; 
    }

    outputDeviceIndex++;
}
//gather input devices
static void on_dev_source(pa_context *c, const pa_source_info *info, int eol, void *udata){
    
    if (eol != 0){
        pa_threaded_mainloop_signal(mloop, 0);
        return; 
    }

    std::cout << "source: " << inputDeviceIndex << ": "<< info->name << '\n';

    if (inputDeviceIndex == inputDevice){

        inputDeviceID = info->name;

        inputDeviceIndex++;

        pa_threaded_mainloop_signal(mloop, 0);
        return; 
    }

    inputDeviceIndex++;
}
//do and unreference pa operation
static void do_op(pa_operation * op){

    for(;;){
        int r = pa_operation_get_state(op);
        
        if (r == PA_OPERATION_DONE || r == PA_OPERATION_CANCELLED) break;

        pa_threaded_mainloop_wait(mloop);
    }

    pa_operation_unref(op);
}
//callback for when new data is available in the stream
static void on_io_complete(pa_stream *s, size_t nbytes, void *udata){

    pa_threaded_mainloop_signal(mloop, 0);
}


AudioManager::AudioManager()
{
    processor = std::make_unique<FFTProcessor>();
    processor->init(SAMPLE_RATE, sizeof(short int), CHANNEL_NUM, BUFFER_LENGTH_MSEC);

    for (int i = 0; i < BINS; i++){
        m_magnitudeData[i] = 0.0;
    }

    processor->bindMagnitudeBuffer(m_magnitudeData);
}

AudioManager::~AudioManager()
{
}

float * AudioManager::getMagnitudeData(){

    return m_magnitudeData;
}

static void getDeviceIDs(){
    //output 
    do_op(pa_context_get_sink_info_list(ctx, on_dev_sink, NULL));
    //input 
    do_op(pa_context_get_source_info_list(ctx, on_dev_source, NULL));

    std::cout << "in: "<< inputDeviceID << '\n';
    std::cout << "out: "<< outputDeviceID << '\n';
}

//start the audio recording/processing in a new thread
static void start(pa_context *ctx, std::shared_ptr<AudioManager> audioManager){

    //create and set up stream
    pa_sample_spec spec;
    spec.format = PA_SAMPLE_S16LE;
    spec.rate = SAMPLE_RATE;
    spec.channels = CHANNEL_NUM;
    pa_stream *stm = pa_stream_new(ctx, "aenc", &spec, NULL);

    int sampleSize = sizeof(short int);

    pa_buffer_attr attr;
    memset(&attr, 0xff, sizeof(attr));
    attr.tlength = spec.rate * sampleSize * spec.channels * BUFFER_LENGTH_MSEC / 1000; //set buffer max length

    pa_stream_set_read_callback(stm, on_io_complete, NULL); //set callback func for new data available
    pa_stream_connect_record(stm, inputDeviceID.data(), &attr, PA_STREAM_NOFLAGS); //connect to device using device ID

    //wait untill stream is ready
    for (;;) {
        int r = pa_stream_get_state(stm);
        if (r == PA_STREAM_READY) break;
        else if (r == PA_STREAM_FAILED) std::cout << "stream error\n";

        pa_threaded_mainloop_wait(mloop);
    }

    //audio processing loop
    std::cout << "starting audio\n";

    while (audioRunning){
        const void * data;
        size_t n;

        pa_stream_peek(stm, &data, &n);

        if(n == 0) {
            //buffer empty
            pa_threaded_mainloop_wait(mloop);
            continue;
        } else if (data == NULL && n != 0) {
            std::cout << "buffer overrun\n";
        } else {
            audioManager->processor->process((short int *)data, n);
        }

        pa_stream_drop(stm);
    }

    pa_stream_disconnect(stm);
    pa_stream_unref(stm);
}

void setupAudio(std::shared_ptr<AudioManager> audioManager){

    mloop = pa_threaded_mainloop_new();

    // create/start loop and lock
    pa_threaded_mainloop_start(mloop);
    pa_threaded_mainloop_lock(mloop);

    mlapi = pa_threaded_mainloop_get_api(mloop); //get api
    ctx = pa_context_new_with_proplist(mlapi, "aenc", NULL); //get context with application name and property list
    pa_context_set_state_callback(ctx, on_state_change, NULL); // set context state change call back function

    /* Connect the context */
    if (pa_context_connect(ctx, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0) {
        fprintf(stderr, "pa_context_connect() failed.\n");
        return;
    }

    //wait untill context is ready
    while (pa_context_get_state(ctx) != PA_CONTEXT_READY){
        pa_threaded_mainloop_wait(mloop);
    }

    //set the device ID strings using the device number
    getDeviceIDs();
    //start the audio recording/processing in a new thread
    start(ctx, audioManager);

    //closing
    pa_context_disconnect(ctx);
    pa_context_unref(ctx);

    pa_threaded_mainloop_unlock(mloop);
    pa_threaded_mainloop_stop(mloop);
    pa_threaded_mainloop_free(mloop);

    threadComplete = true;

    return;
}
