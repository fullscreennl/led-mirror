#ifndef LEDMIRROR_H
#define LEDMIRROR_H

extern const int imagewidth;
extern const int imageheight;

void displayImage(int *pixelstodraw, int length);
int ledmirror_run();
int pack(int p1,int p2,int p3,int p4);
#endif
