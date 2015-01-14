#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ledmirror.h"
#include "utils.h"
#include "menu_overlay.h"

#include "menu.h"
#include "looper.h"

#define BUFFER_SIZE 6144
#define THRESHOLD 50

typedef enum{
    appStateMenu = 1,
    appStateLooper = 2,
    appSateApp2 = 3,
    appSateApp3 = 4
}AppState;

static unsigned clearframe[2048] = {0};

static unsigned sensor_1[16] = {276,277,278,279,
                                340,341,342,343,
                                404,405,406,407,
                                468,469,470,471};

static unsigned sensor_2[16] = {};
static unsigned sensor_3[16] = {};

float prevAverage = 0;

AppState appState = appStateMenu;

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

void returnToMenu(){
    appState = appStateMenu;
}

int readSensorState(MMAL_BUFFER_HEADER_T *buffer, int framecounter){
    int i;
    float totalPixelValues = 0.0;
    for(i=0;i<16;i++){
        int pixelindex = sensor_1[i];
        char pixelValue = buffer->data[pixelindex];
        totalPixelValues += pixelValue;
    }    
    float avg = totalPixelValues/16.0;

    if(prevAverage == 0){
        prevAverage = avg;
    }else{
        float delta = fabs(avg-prevAverage);
        if(delta > THRESHOLD){
            printf("TRIGGER %f \n",delta);
            return 1;
        }
        prevAverage = (prevAverage+avg) / 2.0;
    }
    return 0;
}

void videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer, int framecounter){
    if(appState == appStateLooper){
        looper_videoFrameDidRender(buffer,framecounter);
        return;
    }
    int didTrigger = readSensorState(buffer,framecounter);
    if(didTrigger){
       prevAverage = 0;
       looper_init();
       appState = appStateLooper; 
    }
}

void videoFrameWillRender(int framecounter){
    if(appState == appStateLooper){
        looper_videoFrameWillRender(framecounter);
        return;
    }
    displayImage(sensor_overlay);
}

int main(int argc, const char **argv)
{
    setDisplayMode(displayModeVideoAndOverlay);
    int exitcode = ledmirror_run();
    return exitcode;
}

