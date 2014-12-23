#include "ledmirror.h"
#include "utils.h"
#include "animation.h"

static int mycounter = 0;


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


void videoFrameDidRender(int framecounter){

    if(framecounter > 300){
        //setDisplayVideo(0);
    }

    /* 
    int frc = framecounter%(1900/32);
    int offset = frc * 32;
    int xoffset = frc%32-15; 
   
    static unsigned image[2048] = {0};
    int i;
    for (i=0; i<2048; i++){
        image[i] = 0;
    }

    int pixvalue = 151;

    image[15 + offset + xoffset] = pixvalue; 
    image[16 + offset + xoffset] = pixvalue;
    image[47 + offset + xoffset] = pixvalue; 
    image[48 + offset + xoffset] = pixvalue; 
    image[77 + offset + xoffset] = pixvalue;
    image[78 + offset + xoffset] = pixvalue;
    image[79 + offset + xoffset] = pixvalue;
    image[80 + offset + xoffset] = pixvalue;
    image[81 + offset + xoffset] = pixvalue;
    image[82 + offset + xoffset] = pixvalue; 
    image[109 + offset + xoffset] = pixvalue;
    image[110 + offset + xoffset] = pixvalue;
    image[111 + offset + xoffset] = pixvalue;
    image[112 + offset + xoffset] = pixvalue;
    image[113 + offset + xoffset] = pixvalue;
    image[114 + offset + xoffset] = pixvalue; 
    image[143 + offset + xoffset] = pixvalue;
    image[144 + offset + xoffset] = pixvalue;
    image[175 + offset + xoffset] = pixvalue;
    image[176 + offset + xoffset] = pixvalue; 
    */
        
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
    int exitcode = ledmirror_run();
    return exitcode;
}

