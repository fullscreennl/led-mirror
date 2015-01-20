#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_buffer.h"

#ifndef LEDMIRROR_H
#define LEDMIRROR_H

typedef enum{
    displayModeVideoAndOverlay = 1,
    displayModeOverlay = 2,
    displayModePlayback = 3

}DisplayMode;

extern const int imagewidth;
extern const int imageheight;

//displays a superimposed image of 2048 pixels
void displayImage(unsigned int *pixelstodraw);

//plays back a recorded or constructed frame buffer
void playbackFrame(void *data);

//sets the display mode
//  displayModeVideoAndOverlay = 1,
//  displayModeOverlay = 2,
//  displayModePlayback = 3
void setDisplayMode(DisplayMode mode);

//starts the capturing and dispatching of video events
int ledmirror_run();

//packs the state of 4 pixels, pixels are 2 bits = 4 intensity levels
//this is normally only used for animations that do not supply full frame buffers
//NOTE: the pack function will call the quantize function in the delegate before packing 
//
//example:
//
//        int p4 = 0;
//        int p3 = 90;
//        int p2 = 120;
//        int p1 = 240;
//        int packed_4_pix = pack(p1,p2,p3,p4);
//
//        char output_buffer[3];
//        output_buffer[0] = 0x00;
//        output_buffer[1] = 0;
//        output_buffer[2] = packed_4_pix;
//        bcm2835_spi_transfern(&output_buffer[0], 3);
//
//this example will set the top-left column of 4 pixels
//
int pack(int p1,int p2,int p3,int p4);

//this protocol should be implemented by apps/code 
//calling ledmirror_run() for handling video events
//  
//  int quantize(int level);
//  void videoFrameWillRender(int framecounter);
//  void videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer,int framecounter);
//  

//incomming between 0-255 output between 0-3
int quantize(int level);

//triggered before a video frame will render
void videoFrameWillRender(int framecounter);

//triggered after a video frame did render
//we have access to the pixel buffer here to manipulate or record video frames
void videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer,int framecounter);

#endif
