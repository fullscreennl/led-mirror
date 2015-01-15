#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ledmirror.h"
#include "utils.h"
#include "animation.h"
#include "math.h"

#include "menu.h"

#define CAPTURE_LENGTH 1
#define BUFFER_SIZE 6144
#define COUNTDOWN_SEQ_LENGTH 77

int differClock = 0;
static int recordedCounter = 0;
static int playbackCounter = 0;
static int countDownClock = 0;
static int APP_DURATION = 200;
int diffBufferInitialized = 0;
uint8_t *displayBuffer;
static void *recordedBuffers[CAPTURE_LENGTH];

static unsigned clearframe[2048] = {0};

int differ_quantize(int level)
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

void createBuffers(){
    if(diffBufferInitialized){
        return;
    }
    uint8_t *diffBuffer = malloc(BUFFER_SIZE);
    displayBuffer = malloc(BUFFER_SIZE);
    recordedBuffers[0] = diffBuffer;
    diffBufferInitialized = 1;
}

void differ_createOutputVideo(uint8_t* inputbuffer, uint8_t* refbuffer, uint8_t* outputBuffer){
    int i;
    for(i =0; i< BUFFER_SIZE;i++){
        outputBuffer[i] = rand()%255;
    }
}

void differ_videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer, int framecounter){
    //countdown sequence takes 77 frames, after that record diff frame
    if(recordedCounter < CAPTURE_LENGTH && differClock > COUNTDOWN_SEQ_LENGTH){
        memcpy(recordedBuffers[0],buffer->data,BUFFER_SIZE); 
        recordedCounter ++;
        displayImage(frame7);
    }
    //clearing overlay and setting displaymode on playback
    if(differClock == (COUNTDOWN_SEQ_LENGTH + CAPTURE_LENGTH)){
        displayImage(clearframe);
        setDisplayMode(displayModePlayback);
    }

    if(differClock >= (COUNTDOWN_SEQ_LENGTH + CAPTURE_LENGTH)){
        differ_createOutputVideo(buffer->data, recordedBuffers[0], &displayBuffer); 
        playbackFrame(displayBuffer);    
        playbackCounter ++;    
    }

}



void differ_videoFrameWillRender(int framecounter){

    differClock ++;
    
    if(differClock%7 == 0 && countDownClock < 11){
        countDownClock ++;
    }
    
    //return to main menu after APP DURATION
    if(differClock == (COUNTDOWN_SEQ_LENGTH + CAPTURE_LENGTH + APP_DURATION)){
        setDisplayMode(displayModeVideoAndOverlay);
        playbackCounter = 0;
        differClock = 0;
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
    if(recordedCounter < CAPTURE_LENGTH && differClock > 77){
        if(differClock%6 == 0){
            displayImage(frame7);
        }else{
            displayImage(frame8);
        }
    }

}

void differ_init()
{
    
    differClock = 0;
    recordedCounter = 0;
    playbackCounter = 0;
    countDownClock = 0;
    
    //createRecordingBuffer();
    createDiffBuffer();
}

