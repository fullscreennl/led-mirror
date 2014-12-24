#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ledmirror.h"
#include "utils.h"
#include "animation.h"

#define RECORDING_LENGTH 150
#define BUFFER_SIZE 6144

static int mycounter = 0;

static void *recordedBuffers[RECORDING_LENGTH];
int recordedCounter = 0;

int quantize(int level)
{
    int output_pixel;
    if(level < 50){
        output_pixel = 0;
    }else if(level < 150){
        output_pixel = 1;
    }else if(level < 220){
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
    if(framecounter < RECORDING_LENGTH){
        memcpy(recordedBuffers[recordedCounter],buffer->data,BUFFER_SIZE); 
        recordedCounter ++;
    }

    if(framecounter == 150){
        for (i=0; i<2048; i++){
            fr[i] = 1;
        }
        displayImage(fr);
    }    

    if(framecounter == 153){
        setDisplayMode(displayModePlayback);
    }

    if(framecounter > 180){
        playbackFrame(recordedBuffers[framecounter%149]);        
        printf("playback frame %i\n",framecounter%149);
    }

}


void videoFrameWillRender(int framecounter){

    static unsigned clearframe[2048] = {0};

    if(framecounter%15 == 0 && mycounter < 11){
        mycounter ++;
    }

    if(framecounter%300 == 0 && mycounter < 13){
        mycounter ++;
    }

    if(mycounter >= 13){
        mycounter = 0;
    }

    void *frames[14];

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
    frames[12] = frame7;
    frames[13] = clearframe;

    displayImage(frames[mycounter]);

}

int main(int argc, const char **argv)
{
    createRecordingBuffer();
    int exitcode = ledmirror_run();
    return exitcode;
}

