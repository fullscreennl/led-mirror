#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ledmirror.h"
#include "utils.h"
#include "animation.h"

#define RECORDING_LENGTH 150
#define BUFFER_SIZE 6144

int loopingClock = 0;
int recordedCounter = 0;
int playbackCounter = 0;
int countDownClock = 0;

static void *recordedBuffers[RECORDING_LENGTH];
static unsigned clearframe[2048] = {0};

int quantize(int level)
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
    int i;
    for(i=0; i< RECORDING_LENGTH; i++){
        uint8_t *newbuffer = malloc(BUFFER_SIZE);
        recordedBuffers[i] = newbuffer;
    }
}

void videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer, int framecounter){
    
    if(recordedCounter < RECORDING_LENGTH && loopingClock > 77){
        memcpy(recordedBuffers[recordedCounter],buffer->data,BUFFER_SIZE); 
        recordedCounter ++;
        displayImage(frame7);
    }

    if(loopingClock == 227){
        displayImage(clearframe);
        setDisplayMode(displayModePlayback);
    }

    if(loopingClock >= 227 && playbackCounter < RECORDING_LENGTH){
        playbackFrame(recordedBuffers[149-playbackCounter]);    
        playbackCounter ++;    
    }

}


void videoFrameWillRender(int framecounter){

    loopingClock ++;
    
    if(loopingClock%7 == 0 && countDownClock < 11){
        countDownClock ++;
    }

    if(loopingClock == 377){
        setDisplayMode(displayModeVideoAndOverlay);
        playbackCounter = 0;
        loopingClock = 0;
        countDownClock = 0;
        recordedCounter = 0;
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
    if(recordedCounter < RECORDING_LENGTH && loopingClock > 77){
        if(loopingClock%6 == 0){
            displayImage(frame7);
        }else{
            displayImage(frame8);
        }
    }

}

int main(int argc, const char **argv)
{
    createRecordingBuffer();
    int exitcode = ledmirror_run();
    return exitcode;
}

