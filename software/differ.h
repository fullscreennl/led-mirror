#ifndef DIFFER_H
#define DIFFER_H

int differ_quantize(int level);
void differ_videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer, int framecounter);
void differ_videoFrameWillRender(int framecounter);
void differ_init();

#endif
