#ifndef LOOPER_H
#define LOOPER_H

int looper_quantize(int level);
void looper_videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer, int framecounter);
void looper_videoFrameWillRender(int framecounter);
void looper_init();

#endif
