#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include "ledmirror.h"
#include "utils.h"
#include "menu_overlay.h"

#include "menu.h"
#include "looper.h"
#include "painter.h"
#include "differ.h"

#define BUFFER_SIZE 6144
#define THRESHOLD 50

typedef enum{
    appStateMenu = 1,
    appStateLooper = 2,
    appStatePainter = 3,
    appStateDifference = 4
}AppState;

static unsigned clearframe[2048] = {0};

static unsigned sensor_1[16] = {276,277,278,279,
                                340,341,342,343,
                                404,405,406,407,
                                468,469,470,471};

static unsigned sensor_2[16] = {286,287,288,289,
                                350,351,352,353,
                                414,415,416,417,
                                478,479,480,481};

static unsigned sensor_3[16] = {296,297,298,299,
                                360,361,362,363,
                                424,425,426,427,
                                488,489,490,491};

float avg1 = 0;
float avg2 = 0;
float avg3 = 0;
float *prevAverage_sensor1 = &avg1;
float *prevAverage_sensor2 = &avg2;
float *prevAverage_sensor3 = &avg3;

AppState appState = appStateMenu;

int quantize(int level)
{
    if(appState == appStatePainter){
        return painter_quantize(level);
    }
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

int readSensorState(unsigned sensor[],float *prevAverage, MMAL_BUFFER_HEADER_T *buffer, int framecounter){
    int i;
    float totalPixelValues = 0.0;
    for(i=0;i<16;i++){
        int pixelindex = sensor[i];
        char pixelValue = buffer->data[pixelindex];
        totalPixelValues += pixelValue;
    }    
    float avg = totalPixelValues/16.0;

    if(*prevAverage == 0){
        *prevAverage = avg;
    }else{
        float delta = fabs(avg-*prevAverage);
        if(delta > THRESHOLD){
            printf("TRIGGER %f \n",delta);
            return 1;
        }
        float newavg = (*prevAverage+avg) / 2.0;
        *prevAverage = newavg;
    }
    return 0;
}

void videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer, int framecounter){
    
    if(appState == appStateLooper){
        looper_videoFrameDidRender(buffer,framecounter);
        return;
    }
    if(appState == appStatePainter){
        painter_videoFrameDidRender(buffer,framecounter);
        return;
    }
    if(appState == appStateDifference){
        differ_videoFrameDidRender(buffer,framecounter);
        return;
    }

    int sensor_1_didTrigger = readSensorState(sensor_1,prevAverage_sensor1,buffer,framecounter);
    if(sensor_1_didTrigger){
       *prevAverage_sensor1 = 0;
       looper_init();
       appState = appStateLooper;
       return; 
    }

    int sensor_2_didTrigger = readSensorState(sensor_2,prevAverage_sensor2,buffer,framecounter);
    if(sensor_2_didTrigger){
       *prevAverage_sensor2 = 0;
       painter_init();
       appState = appStatePainter;
       return; 
    }

    int sensor_3_didTrigger = readSensorState(sensor_3,prevAverage_sensor3,buffer,framecounter);
    if(sensor_3_didTrigger){
       *prevAverage_sensor3 = 0;
       differ_init();
       appState = appStateDifference;
       return; 
    }

}

void videoFrameWillRender(int framecounter){
    if(appState == appStateLooper){
        looper_videoFrameWillRender(framecounter);
        return;
    }
    if(appState == appStatePainter){
        painter_videoFrameWillRender(framecounter);
        return;
    }
    if(appState == appStateDifference){
        differ_videoFrameWillRender(framecounter);
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

