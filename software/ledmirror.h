#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_buffer.h"

#ifndef LEDMIRROR_H
#define LEDMIRROR_H

extern const int imagewidth;
extern const int imageheight;

void displayImage(unsigned int *pixelstodraw);
void setDisplayVideo(int shouldDisplay);
int ledmirror_run();
int pack(int p1,int p2,int p3,int p4);

//implemented by main.c
int quantize(int level);
void videoFrameWillRender(int framecounter);
void videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer);

#endif
