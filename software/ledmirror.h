#ifndef LEDMIRROR_H
#define LEDMIRROR_H

extern const int imagewidth;
extern const int imageheight;

void displayImage(unsigned int *pixelstodraw, int length);
int ledmirror_run();
int pack(int p1,int p2,int p3,int p4);

//implemented by main.c
int quantize(int level);
void videoFrameDidRender(int framecounter);

#endif
