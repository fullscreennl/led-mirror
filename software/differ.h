#ifndef DIFFER_H
#define DIFFER_H

//Implementation of the ledmirror protocol, so the video events can be forwarded
//if this app is selected.

int differ_quantize(int level);
void differ_videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer, int framecounter);
void differ_videoFrameWillRender(int framecounter);
void differ_init();

#endif
