//
//This app copies the highlights of the video frames to 
//a frame buffer, so the spectator can 'paint/draw' with 
//highlights, for instance a flash light.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ledmirror.h"
#include "utils.h"
#include "menu.h"

#define BUFFER_SIZE 6144

static unsigned clearframe[2048] = {0};
static int loopingClock = 0;
static int appDuration = 300;
static int brightnessThreshold = 240;

static int bufferInitialized = 0;
uint8_t *displayBuffer;


static void createBuffers(){
    if(bufferInitialized){
        return;
    }
    displayBuffer = malloc(BUFFER_SIZE);
    bufferInitialized = 1;
}

static void clearPainting(){
    int i;
    for(i =0; i< BUFFER_SIZE;i++){
        displayBuffer[i] = 0;
    }
}

//copies pixels brighter than threshold to the painting
void painter_createOutputVideo(uint8_t* inputbuffer, uint8_t* outputBuffer){
    int i;
    for(i =0; i< BUFFER_SIZE;i++){
        int pix = 0;
        if (inputbuffer[i] > brightnessThreshold){
            pix = 255;
        }
        pix = outputBuffer[i] + pix;
        if(pix > 255){
            pix = 255;
        }
        outputBuffer[i] = pix;
    }
}

void painter_videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer, int framecounter){
    painter_createOutputVideo(buffer->data, displayBuffer); 
    playbackFrame(displayBuffer);   
}

void painter_videoFrameWillRender(int framecounter){

    loopingClock ++;
    
    if(loopingClock == appDuration){
        setDisplayMode(displayModeVideoAndOverlay);
        loopingClock = 0;
        returnToMenu();
        return;
    }

}

void painter_init()
{
    displayImage(clearframe);
    createBuffers();
    clearPainting();
    setDisplayMode(displayModePlayback);
    loopingClock = 0;
}

