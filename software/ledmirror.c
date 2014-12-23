//This is a modified version of CAMVC.C - http://www.cheerfulprogrammer.com/downloads/camcv.c

//Info using spi - http://www.raspberry-projects.com/pi/programming-in-c/spi/using-the-spi-interface

/*MODIFIED RASPIVID TO CAMCV.C
This is just a very stripped down version of raspivid (copy right info below), combined with some work from Pierre Raufast, specifically http://raufast.org/download/camcv_vid0.c. 
All it currently does is:
- startup the camera with a preview
- capture the actual memory for each frame of the video
- dump it to disk after 1s
- print out its size every 1s
- shut down after 30s
It's the first step to a reworking to come next that'll wrap up the camera in a slightly more useable api
*/

/*
Copyright (c) 2013, Broadcom Europe Ltd
Copyright (c) 2013, James Hughes
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
 * \file RaspiVid.c
 * Command line program to capture a camera video stream and encode it to file.
 * Also optionally display a preview/viewfinder of current camera input.
 *
 * \date 28th Feb 2013
 * \Author: James Hughes
 *
 * Description
 *
 * 3 components are created; camera, preview and video encoder.
 * Camera component has three ports, preview, video and stills.
 * This program connects preview and stills to the preview and video
 * encoder. Using mmal we don't need to worry about buffers between these
 * components, but we do need to handle buffers from the encoder, which
 * are simply written straight to the file in the requisite buffer callback.
 *
 * We use the RaspiCamControl code to handle the specific camera settings.
 * We use the RaspiPreview code to handle the (generic) preview window
 */

#define RST RPI_V2_GPIO_P1_11

// We use some GNU extensions (basename)
#define _GNU_SOURCE
#include "ledmirror.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <sysexits.h>

#define VERSION_STRING "v1.3.3"

#include "bcm_host.h"
#include "interface/vcos/vcos.h"

#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_logging.h"
#include "interface/mmal/mmal_buffer.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/mmal/util/mmal_util_params.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_connection.h"

#include "pixelmap.h"
#include "utils.h"
#include <semaphore.h>

#include <bcm2835.h>

/// Camera number to use - we only have one camera, indexed from 0.
#define CAMERA_NUMBER 0

// Standard port setting for the camera component
#define MMAL_CAMERA_PREVIEW_PORT 0
#define MMAL_CAMERA_VIDEO_PORT 1
#define MMAL_CAMERA_CAPTURE_PORT 2

// Video format information
#define VIDEO_FRAME_RATE_NUM 15
#define VIDEO_FRAME_RATE_DEN 1

/// Video render needs at least 2 buffers.
#define VIDEO_OUTPUT_BUFFERS_NUM 3

// Max bitrate we allow for recording
const int MAX_BITRATE = 30000000; // 30Mbits/s

/// Interval at which we check for an failure abort during capture
const int ABORT_INTERVAL = 100; // ms

const int captureSize = 64;

const int imagewidth = 16;
const int imageheight = 32;

unsigned int *overlayPixels;
int numOverlayPixels;

static void signal_handler(int signal_number);

// Forward
typedef struct RASPIVID_STATE_S RASPIVID_STATE;


/** Struct used to pass information in encoder port userdata to callback
 */
typedef struct
{
    RASPIVID_STATE *pstate;              /// pointer to our state in case required in callback
    int abort;                           /// Set to 1 in callback if an error occurs to attempt to abort the capture
} PORT_USERDATA;

/** Structure containing all state information for the current run
 */
struct RASPIVID_STATE_S
{
    int timeout;                        /// Time taken before frame is grabbed and app then shuts down. Units are milliseconds
    int width;                          /// Requested width of image
    int height;                         /// requested height of image
    int bitrate;                        /// Requested bitrate
    int framerate;                      /// Requested frame rate (fps)
    int verbose;                        /// !0 if want detailed run information

    int framescaptured;

    MMAL_COMPONENT_T *camera_component;    /// Pointer to the camera component

    MMAL_POOL_T *video_pool; /// Pointer to the pool of buffers used by video output port

    PORT_USERDATA callback_data;        /// Used to move data to the encoder callback
};

/**
 * Assign a default set of parameters to the state passed in
 *
 * @param state Pointer to state structure to assign defaults to
 */
static void default_status(RASPIVID_STATE *state)
{
    if (!state)
    {
        vcos_assert(0);
        return;
    }

    // Default everything to zero
    memset(state, 0, sizeof(RASPIVID_STATE));

    // Now set anything non-zero
    state->timeout = -1;     // 5s delay before take image
    state->width = captureSize;       // Default to 1080p
    state->height = captureSize;
    state->bitrate = 17000000; // This is a decent default bitrate for 1080p
    state->framerate = VIDEO_FRAME_RATE_NUM;
    state->verbose = 0;
    state->framescaptured = 0;
}


/**
 *  buffer header callback function for camera control
 *
 *  Callback will dump buffer data to the specific file
 *
 * @param port Pointer to port from which callback originated
 * @param buffer mmal buffer header pointer
 */
static void camera_control_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
    if (buffer->cmd == MMAL_EVENT_PARAMETER_CHANGED)
    {
    }
    else
    {
        vcos_log_error("Received unexpected camera control callback event, 0x%08x", buffer->cmd);
    }

    mmal_buffer_header_release(buffer);
}

int pack(int p1,int p2,int p3,int p4){

    int pixel_1 = quantize(p1) << 6;
    int pixel_2 = quantize(p2) << 4;
    int pixel_3 = quantize(p3) << 2;
    int pixel_4 = quantize(p4);
    return pixel_1+pixel_2+pixel_3+pixel_4;

}

static void spi_transferVideo(char *buffer,int index){
    bcm2835_spi_transfern(&buffer[0], 3);
}

//merge the overlay data onto the videoframe
static int pixelForIndex(int rawIndex, int videopixel){
    int y = floor(rawIndex/captureSize);
    int x = rawIndex%captureSize - 16;
    int index = y*32+x;
    int pixel = overlayPixels[index];
    if(pixel == 0){
        return videopixel;
    }
    return pixel;
}

static void renderVidSection(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer, int xOffset,int section){
    
    int i;
    int j = 0;
    int realimagewidth = captureSize;
    int rowcount = 0;
    int clusterIndexPadding = imagewidth * imageheight/4;  
    int top_padding = (captureSize - 2*imageheight) * 64;
    if(xOffset > 0){
        clusterIndexPadding = 0;
    }
    if(section == 0x01){
        top_padding = ((captureSize - 2*imageheight) + imageheight) * 64;
    }
    
    j = 0;
    rowcount = 0;
    for(i=0; i < (imagewidth*imageheight/4); i++){
            
            int z = (16 - (i%imagewidth)) + top_padding + 16;
            
            int ind = (z)+j+xOffset;
            int p4 = pixelForIndex(ind, buffer->data[ind]);

            ind = (z)+j+realimagewidth+xOffset;
            int p3 = pixelForIndex(ind, buffer->data[ind]);

            ind = (z)+j+realimagewidth*2+xOffset;
            int p2 = pixelForIndex(ind, buffer->data[ind]);

            ind = (z)+j+realimagewidth*3+xOffset; 
            int p1 = pixelForIndex(ind, buffer->data[ind]);

            int packed_4_pix = pack(p1,p2,p3,p4);

            char output_buffer2[3];
            output_buffer2[0] = section;
            output_buffer2[1] = i+clusterIndexPadding;
            output_buffer2[2] = packed_4_pix;
            spi_transferVideo(&output_buffer2[0], section * 256 + output_buffer2[1]);

            if (i%imagewidth == (imagewidth-1)){
                j = j + realimagewidth*4;
                rowcount ++;
            }
    }

}

static void renderVid(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer){

    int xOffset = 0;
    renderVidSection(port,buffer,xOffset,0x00);
        
    xOffset = 16;
    renderVidSection(port,buffer,xOffset,0x00);

    xOffset = 0;
    renderVidSection(port,buffer,xOffset,0x01);

    xOffset = 16;
    renderVidSection(port,buffer,xOffset,0x01);

}


void displayImage(unsigned int *pixelstodraw, int length){
    overlayPixels = pixelstodraw;
    numOverlayPixels = length;
}


/**
 *  buffer header callback function for vieo
 *
 *  Callback will dump buffer data to the specific file
 *
 * @param port Pointer to port from which callback originated
 * @param buffer mmal buffer header pointer
 */
static void video_buffer_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
    MMAL_BUFFER_HEADER_T *new_buffer;

    // We pass our file handle and other stuff in via the userdata field.

    PORT_USERDATA *pData = (PORT_USERDATA *)port->userdata;

    if (pData)
    {
        int bytes_written = buffer->length;

        if (buffer->length)
        {
            mmal_buffer_header_mem_lock(buffer);
            pData->pstate->framescaptured++;

            videoFrameDidRender(pData->pstate->framescaptured);
            renderVid(port,buffer);

            mmal_buffer_header_mem_unlock(buffer);
        }

        if (bytes_written != buffer->length)
        {
            vcos_log_error("Failed to write buffer data (%d from %d)- aborting", bytes_written, buffer->length);
            pData->abort = 1;
        }
    }
    else
    {
        vcos_log_error("Received a encoder buffer callback with no state");
    }

    // release buffer back to the pool
    mmal_buffer_header_release(buffer);

    // and send one back to the port (if still open)
    if (port->is_enabled)
    {
        MMAL_STATUS_T status  = -1;

        new_buffer = mmal_queue_get(pData->pstate->video_pool->queue);

        if (new_buffer)
            status = mmal_port_send_buffer(port, new_buffer);

        if (!new_buffer || status != MMAL_SUCCESS)
            vcos_log_error("Unable to return a buffer to the video port");
    }
}


/**
 * Create the camera component, set up its ports
 *
 * @param state Pointer to state control struct
 *
 * @return MMAL_SUCCESS if all OK, something else otherwise
 *
 */
static MMAL_STATUS_T create_camera_component(RASPIVID_STATE *state)
{
    MMAL_COMPONENT_T *camera = 0;
    MMAL_ES_FORMAT_T *format;
    MMAL_PORT_T *video_port = NULL, *still_port = NULL;
    MMAL_STATUS_T status;

    /* Create the component */
    status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &camera);

    if (status != MMAL_SUCCESS)
    {
        vcos_log_error("Failed to create camera component");
        goto error;
    }

    if (!camera->output_num)
    {
        status = MMAL_ENOSYS;
        vcos_log_error("Camera doesn't have output ports");
        goto error;
    }

    video_port = camera->output[MMAL_CAMERA_VIDEO_PORT];
    still_port = camera->output[MMAL_CAMERA_CAPTURE_PORT];

    // Enable the camera, and tell it its control callback function
    status = mmal_port_enable(camera->control, camera_control_callback);

    if (status != MMAL_SUCCESS)
    {
        vcos_log_error("Unable to enable control port : error %d", status);
        goto error;
    }

    //  set up the camera configuration
    {
        MMAL_PARAMETER_CAMERA_CONFIG_T cam_config =
        {
         { MMAL_PARAMETER_CAMERA_CONFIG, sizeof(cam_config) },
         .max_stills_w = state->width,
         .max_stills_h = state->height,
         .stills_yuv422 = 1,
         .one_shot_stills = 0,
         .max_preview_video_w = state->width,
         .max_preview_video_h = state->height,
         .num_preview_video_frames = 3,
         .stills_capture_circular_buffer_height = 0,
         .fast_preview_resume = 0,
         .use_stc_timestamp = MMAL_PARAM_TIMESTAMP_MODE_RESET_STC
        };
        mmal_port_parameter_set(camera->control, &cam_config.hdr);
    }

    if (status != MMAL_SUCCESS)
    {
        vcos_log_error("camera viewfinder format couldn't be set");
        goto error;
    }

    // Set the encode format on the video  port

    format = video_port->format;

    format->encoding = MMAL_ENCODING_I420;
    format->encoding_variant = MMAL_ENCODING_I420;

    format->es->video.width = state->width;
    format->es->video.height = state->height;
    format->es->video.crop.x = 0;
    format->es->video.crop.y = 0;
    format->es->video.crop.width = state->width;
    format->es->video.crop.height = state->height;
    format->es->video.frame_rate.num = state->framerate;
    format->es->video.frame_rate.den = VIDEO_FRAME_RATE_DEN;

    status = mmal_port_format_commit(video_port);

    if (status != MMAL_SUCCESS)
    {
        vcos_log_error("camera video format couldn't be set");
        goto error;
    }

    // Ensure there are enough buffers to avoid dropping frames
    if (video_port->buffer_num < VIDEO_OUTPUT_BUFFERS_NUM)
        video_port->buffer_num = VIDEO_OUTPUT_BUFFERS_NUM;


    // Set the encode format on the still  port

    format = still_port->format;

    format->encoding = MMAL_ENCODING_OPAQUE;
    format->encoding_variant = MMAL_ENCODING_I420;

    format->es->video.width = state->width;
    format->es->video.height = state->height;
    format->es->video.crop.x = 0;
    format->es->video.crop.y = 0;
    format->es->video.crop.width = state->width;
    format->es->video.crop.height = state->height;
    format->es->video.frame_rate.num = 1;
    format->es->video.frame_rate.den = 1;

    status = mmal_port_format_commit(still_port);

    if (status != MMAL_SUCCESS)
    {
        vcos_log_error("camera still format couldn't be set");
        goto error;
    }

    //PR : create pool of message on video port
    MMAL_POOL_T *pool;
    // Make a buffer big enough for ARGB data to come back
    video_port->buffer_size = video_port->buffer_size_recommended;
    printf("Creating video port pool with %d buffers of size %d\n", video_port->buffer_num, video_port->buffer_size);
    pool = mmal_port_pool_create(video_port, video_port->buffer_num, video_port->buffer_size);
    if (!pool)
    {
        vcos_log_error("Failed to create buffer header pool for video output port");
    }
    state->video_pool = pool;

    /* Ensure there are enough buffers to avoid dropping frames */
    if (still_port->buffer_num < VIDEO_OUTPUT_BUFFERS_NUM)
        still_port->buffer_num = VIDEO_OUTPUT_BUFFERS_NUM;

    /* Enable component */
    status = mmal_component_enable(camera);

    if (status != MMAL_SUCCESS)
    {
        vcos_log_error("camera component couldn't be enabled");
        goto error;
    }

    state->camera_component = camera;

    if (state->verbose)
        fprintf(stderr, "Camera component done\n");

    return status;

error:

    if (camera)
        mmal_component_destroy(camera);

    return status;
}

/**
 * Destroy the camera component
 *
 * @param state Pointer to state control struct
 *
 */
static void destroy_camera_component(RASPIVID_STATE *state)
{
   if (state->camera_component)
   {
        mmal_component_destroy(state->camera_component);
        state->camera_component = NULL;
   }
}

/**
 * Checks if specified port is valid and enabled, then disables it
 *
 * @param port  Pointer the port
 *
 */
static void check_disable_port(MMAL_PORT_T *port)
{
    if (port && port->is_enabled)
        mmal_port_disable(port);
}

/**
 * Handler for sigint signals
 *
 * @param signal_number ID of incoming signal.
 *
 */
static void signal_handler(int signal_number)
{
   if (signal_number == SIGUSR1)
   {
        // Handle but ignore - prevents us dropping out if started in none-signal mode
        // and someone sends us the USR1 signal anyway
   }
   else
   {
        // Going to abort on all other signals
        vcos_log_error("Aborting program\n");
        exit(130);
   }

}

/**
 * main
 */
//int main(int argc, const char **argv)
int ledmirror_run()
{
   
    if (!bcm2835_init())
        return -1;

    bcm2835_gpio_fsel(RST, BCM2835_GPIO_FSEL_OUTP);

    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256);    // The default //64
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default

    bcm2835_gpio_write(RST, HIGH);
    usleep(5000);
    bcm2835_gpio_write(RST, LOW);
    usleep(5000);
    bcm2835_gpio_write(RST, HIGH);

    //Panel Configuration Register
    char output_buffer[2];
    output_buffer[0] = 0x0D;
    output_buffer[1] = 0x81;
    bcm2835_spi_transfern(&output_buffer[0], 2);

    //Panel Intensity Register
    output_buffer[0] = 0x02;
    output_buffer[1] = 0x30;
    bcm2835_spi_transfern(&output_buffer[0], 2);

    //Number of Cascaded Devices Register  
    output_buffer[0] = 0x0e;
    output_buffer[1] = 0x0F;
    bcm2835_spi_transfern(&output_buffer[0], 2);

    //Number of Display Rows Register  
    output_buffer[0] = 0x0f;
    output_buffer[1] = 0x00;
    bcm2835_spi_transfern(&output_buffer[0], 2);

    usleep(500000);

    // Our main data storage vessel..
    RASPIVID_STATE state;
    int exit_code = EX_OK;

    MMAL_STATUS_T status = MMAL_SUCCESS;
    MMAL_PORT_T *camera_video_port = NULL;
    MMAL_PORT_T *camera_still_port = NULL;

    bcm_host_init();

    // Register our application with the logging system
    vcos_log_register("RaspiVid", VCOS_LOG_CATEGORY);

    signal(SIGINT, signal_handler);

    // Disable USR1 for the moment - may be reenabled if go in to signal capture mode
    signal(SIGUSR1, SIG_IGN);

    default_status(&state);

    // OK, we have a nice set of parameters. Now set up our components
    // We have three components. Camera, Preview and encoder.

    if ((status = create_camera_component(&state)) != MMAL_SUCCESS)
    {
        vcos_log_error("%s: Failed to create camera component", __func__);
        exit_code = EX_SOFTWARE;
    }else{

        if (state.verbose)
            fprintf(stderr, "Starting component connection stage\n");

        camera_video_port   = state.camera_component->output[MMAL_CAMERA_VIDEO_PORT];
        camera_still_port   = state.camera_component->output[MMAL_CAMERA_CAPTURE_PORT];

        if (1)
        {
            if (state.verbose)
                fprintf(stderr, "Connecting camera stills port to encoder input port\n");

            // Set up our userdata - this is passed though to the callback where we need the information.
            state.callback_data.pstate = &state;
            state.callback_data.abort = 0;
            camera_video_port->userdata = (struct MMAL_PORT_USERDATA_T *)&state.callback_data;

            //setup the video buffer callback
            status = mmal_port_enable(camera_video_port, video_buffer_callback);
            if (status != MMAL_SUCCESS)
            {
                vcos_log_error("Failed to setup video buffer callback");
                goto error;
            }

            // Send all the buffers to the vide output port
            {
                int num = mmal_queue_length(state.video_pool->queue);
                int q;
                for (q=0;q<num;q++)
                {
                    MMAL_BUFFER_HEADER_T *buffer = mmal_queue_get(state.video_pool->queue);

                    if (!buffer){
                        vcos_log_error("Unable to get a required buffer %d from pool queue", q);
                    }

                    if (mmal_port_send_buffer(camera_video_port, buffer)!= MMAL_SUCCESS){
                        vcos_log_error("Unable to send a buffer to encoder output port (%d)", q);
                    }

                }
            }

            //begin capture
            if (mmal_port_parameter_set_boolean(camera_video_port, MMAL_PARAMETER_CAPTURE, 1) != MMAL_SUCCESS)
            {
                printf("Failed to start capture\n");
                goto error;
            }
            vcos_sleep(state.timeout);

        }else{
            vcos_log_error("%s: Failed to connect camera to preview", __func__);
        }

    error:

        if (state.verbose)
            fprintf(stderr, "Closing down\n");

        // Disable all our ports that are not handled by connections
        check_disable_port(camera_still_port);

        if (state.camera_component)
            mmal_component_disable(state.camera_component);

        destroy_camera_component(&state);

        if (state.verbose)
            fprintf(stderr, "Close down completed, all components disconnected, disabled and destroyed\n\n");
    }

    bcm2835_spi_end();
    bcm2835_close();

    return exit_code;
}

