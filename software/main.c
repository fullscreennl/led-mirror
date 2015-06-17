//
// This app will display 3 'sensors' at the top of the 
// led installation. (a very simple menu sytem). If motion is detected
// in one of the sensors it will forward the ledmirror protocol events to
// different apps (looper,painter,differ)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include "Config.h"
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

//Do not look for motion in the sensors
//for 10 frames after start up.
//The camera will adjust exposure.
static int initPeriod = 10;

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
float *prevAverage = &avg1;
const char *levels;
//initializes appstate
AppState appState = appStateMenu;

//Quantizes the incomming 255 levels to 4 levels,
//this may need tweaking if lighting conditions change.
int quantize(int level)
{
    int output_pixel;
    levels = Config_getLevels();
    //printf("%i level0",levels[0]);
    if(level < levels[0]){
        output_pixel = 0;
    }else if(level < levels[1]){
        output_pixel = 1;
    }else if(level < levels[2]){
        output_pixel = 2;
    }else{
        output_pixel = 3;
    }
    return output_pixel;
}

void returnToMenu(){
    avg1 = avg2 = avg3 = 0;
    appState = appStateMenu;
}

//changes appstate if a sensor is triggered
int readSensorState(unsigned sensor[],float *avgvar, MMAL_BUFFER_HEADER_T *buffer, int framecounter){
    if(framecounter < initPeriod){
        return 0;
    }
    prevAverage = avgvar;
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

unsigned* getSensor(int ind){
    int newApp =  Config_getApp(ind);
    //printf("app requested: %i on sensor %i\n", newApp, ind);
    if(newApp == 1){
        return sensor_1;
    }else if(newApp == 2){
        return sensor_2;
    }else if(newApp == 3){
        return sensor_3;
    }else{
        return sensor_1;
    }
}

//if the appstate is not 'menu' forwards the events to the selected app
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

    int sensor_1_didTrigger = readSensorState(getSensor(1),&avg1,buffer,framecounter);
    if(sensor_1_didTrigger){
       looper_init();
       appState = appStateLooper;
       return; 
    }

    int sensor_2_didTrigger = readSensorState(getSensor(2),&avg2,buffer,framecounter);
    if(sensor_2_didTrigger){
       painter_init();
       appState = appStatePainter;
       return; 
    }

    int sensor_3_didTrigger = readSensorState(getSensor(3),&avg3,buffer,framecounter);
    if(sensor_3_didTrigger){
       differ_init();
       appState = appStateDifference;
       return; 
    }

}

//displays the menu overlay or forwards to selected app
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
    int odd = 0;
    if((int)(floor((float)framecounter/10.0))%2==0){
        odd = 1;
    }
    if(odd==0){
        displayImage(sensor_overlay2);
    }else{
        displayImage(sensor_overlay1);
    }
}

int setIniBasePath(const char *inifilePath){
    char* token;
    char* string;
    char* tofree;
    string = strdup(inifilePath);
    if(strlen(inifilePath) > 128){
        return -1;
    }
    char base_path[128] = "";
    if (string != NULL) {
        tofree = string;
        while ((token = strsep(&string, "/")) != NULL){
            if(strstr(token, ".ini") == NULL) {
                 strcat(base_path, token);
                 strcat(base_path, "/");
            }
        }
        free(tofree);
    }
    Config_setBasePath(base_path);
    return 0;
}

int main(int argc, const char **argv)
{
    const char *inifile = "settings.ini";
    if(argc == 2){
        inifile = argv[1];
    }

    if (setIniBasePath(inifile) == -1){
        printf("%s inifile name to long.\n", inifile);
        return 1;
    }

    // 1) load the config
    Config_load(inifile);
    levels = Config_getLevels();    
    
    //printf("%i level0",levels[0]);
    setDisplayMode(displayModeVideoAndOverlay);
    int exitcode = ledmirror_run();
    return exitcode;
}

