#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ledmirror.h"
#include "utils.h"
#include "menu.h"


static unsigned clearframe[2048] = {0};
static int loopingClock = 0;

int painter_quantize(int level)
{
    int output_pixel;
    if(level > 200){
        output_pixel = 3;
    }else{
        output_pixel = 0;
    }
    return output_pixel;
}

void painter_videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer, int framecounter){
    
}


void painter_videoFrameWillRender(int framecounter){

    loopingClock ++;
    
    if(loopingClock == 200){
        setDisplayMode(displayModeVideoAndOverlay);
        loopingClock = 0;
        returnToMenu();
        return;
    }

}

void painter_init()
{
    printf("INIT PAINTER\n");
    displayImage(clearframe);
    loopingClock = 0;
}

