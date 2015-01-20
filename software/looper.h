#ifndef LOOPER_H
#define LOOPER_H

//Implementation of the ledmirror protocol, so the video events can be forwarded
//if this app is selected.

int looper_quantize(int level);
void looper_videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer, int framecounter);
void looper_videoFrameWillRender(int framecounter);
void looper_init();

#endif
