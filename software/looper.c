//
//This app records approx 10 seconds of video frames, after a countdown. 
//It will playback the video backwards before returning to the menu.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ledmirror.h"
#include "utils.h"
#include "animation.h"

#include "menu.h"

#define RECORDING_LENGTH 150
#define BUFFER_SIZE 6144

static int loopingClock = 0;
static int recordedCounter = 0;
static int playbackCounter = 0;
static int countDownClock = 0;

//durations in frames
static int appDuration = 377; 
static int countdownDuration = 77;
static int recordingDuration = 227;

int bufferInitialized = 0;

static void *recordedBuffers[RECORDING_LENGTH];
static unsigned clearframe[2048] = {0};

int looper_quantize(int level)
{
    int output_pixel;
    if(level < 50){
        output_pixel = 0;
    }else if(level < 150){
        output_pixel = 1;
    }else if(level < 200){
        output_pixel = 2;
    }else{
        output_pixel = 3;
    }
    return output_pixel;
}


void createRecordingBuffer(){
    if(bufferInitialized){
        return;
    }
    int i;
    for(i=0; i< RECORDING_LENGTH; i++){
        uint8_t *newbuffer = malloc(BUFFER_SIZE);
        recordedBuffers[i] = newbuffer;
    }
    bufferInitialized = 1;
}

void looper_videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer, int framecounter){
    
    //recording
    if(recordedCounter < RECORDING_LENGTH && loopingClock > countdownDuration){
        memcpy(recordedBuffers[recordedCounter],buffer->data,BUFFER_SIZE); 
        recordedCounter ++;
        displayImage(frame7);
    }

    //start playback
    if(loopingClock == recordingDuration){
        displayImage(clearframe);
        setDisplayMode(displayModePlayback);
    }

    //backwards playback
    if(loopingClock >= recordingDuration && playbackCounter < RECORDING_LENGTH){
        playbackFrame(recordedBuffers[(RECORDING_LENGTH-1)-playbackCounter]);    
        playbackCounter ++;    
    }

}


void looper_videoFrameWillRender(int framecounter){

    loopingClock ++;
    
    if(loopingClock%7 == 0 && countDownClock < 11){
        countDownClock ++;
    }

    if(loopingClock == appDuration){
        setDisplayMode(displayModeVideoAndOverlay);
        playbackCounter = 0;
        loopingClock = 0;
        countDownClock = 0;
        recordedCounter = 0;
        returnToMenu();
        return;
    }
    
    void *frames[12];

    frames[0] = frame1;
    frames[1] = clearframe;
    frames[2] = frame2;
    frames[3] = clearframe;
    frames[4] = frame3;
    frames[5] = clearframe;
    frames[6] = frame4;
    frames[7] = clearframe;
    frames[8] = frame5;
    frames[9] = clearframe;
    frames[10] = frame6;
    frames[11] = clearframe;

    displayImage(frames[countDownClock]);
    if(recordedCounter < RECORDING_LENGTH && loopingClock > countdownDuration){
        if(loopingClock%6 == 0){
            displayImage(frame7);
        }else{
            displayImage(frame8);
        }
    }

}

void looper_init()
{
    
    loopingClock = 0;
    recordedCounter = 0;
    playbackCounter = 0;
    countDownClock = 0;
    
    createRecordingBuffer();
}

