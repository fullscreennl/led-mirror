#ifndef PAINTER_H
#define PAINTER_H

//Implementation of the ledmirror protocol, so the video events can be forwarded
//if this app is selected.

int painter_quantize(int level);
void painter_videoFrameDidRender(MMAL_BUFFER_HEADER_T *buffer, int framecounter);
void painter_videoFrameWillRender(int framecounter);
void painter_init();

#endif
