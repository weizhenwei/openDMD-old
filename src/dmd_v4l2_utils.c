/*
 *
 * Copyright (c) 2014, weizhenwei
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the {organization} nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: dmd_v4l2_utils.c
 *
 * Brief: Wrapper functions about v4l2 api originate from <linux/videodev2.h>
 *
 * Date: 2014.05.10
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "dmd_log.h"
#include "dmd_v4l2_utils.h"

/*
 * DRIVER CAPABILITIES
 *
 * struct v4l2_capability {
 * 	__u8	driver[16];	// i.e. "bttv"
 * 	__u8	card[32];	// i.e. "Hauppauge WinTV"
 * 	__u8	bus_info[32];	// "PCI:" + pci_name(pci_dev)
 * 	__u32   version;        // should use KERNEL_VERSION()
 * 	__u32	capabilities;	// Device capabilities,
 * 				// for more details, see linux/videodev2.h
 * 	__u32	reserved[4];
 * };
 *
 * query video device's capability
 */
int video_capability(struct v4l2_device_info *v4l2_info)
{
    int ret = 0;

    // get the device capability.
    struct v4l2_capability *capture = &v4l2_info->cap;
    if ((ret = ioctl(v4l2_info->video_device_fd, VIDIOC_QUERYCAP, capture))
            == -1) {
        dmd_log(LOG_ERR, "ioctl VIDIOC_QUERYBUF error:\n", strerror(errno));
        return ret;
    }

    dmd_log(LOG_INFO, "*******Device Capability informations*******\n");
    dmd_log(LOG_INFO, "driver: %s\n", capture->driver);
    dmd_log(LOG_INFO, "card: %s\n", capture->card);
    dmd_log(LOG_INFO, "bus_info: %s\n", capture->bus_info);
    dmd_log(LOG_INFO, "version: %x\n", capture->version);
    dmd_log(LOG_INFO, "capabilities: 0X%x\n", capture->capabilities);
    if (capture->capabilities & V4L2_CAP_VIDEO_CAPTURE) {
        dmd_log(LOG_INFO, "Capture capability is supported\n");
    } else {
        dmd_log(LOG_INFO, "Capture capability is not supported\n");
    }

    if (capture->capabilities & V4L2_CAP_VIDEO_OUTPUT) {
        dmd_log(LOG_INFO, "Output capability is supported\n");
    } else {
        dmd_log(LOG_INFO, "Output capability is not supported\n");
    }

    if (capture->capabilities & V4L2_CAP_STREAMING) {
        dmd_log(LOG_INFO, "Streaming capability is supported\n");
    } else {
        dmd_log(LOG_INFO, "Streaming capability is not supported\n");
    }

    return ret;
}


/*
 *	VIDEO   INPUTS
 *
 * struct v4l2_input {
 * 	__u32	     index;	//  Which input
 * 	__u8	     name[32];	//  Label
 * 	__u32	     type;	//  Type of input
 * 	__u32	     audioset;	//  Associated audios (bitfield)
 * 	__u32        tuner;     //  Associated tuner
 * 	v4l2_std_id  std;
 * 	__u32	     status;
 * 	__u32	     capabilities;
 * 	__u32	     reserved[3];
 * }
 *
 * query and set video input format
 */
int video_input(struct v4l2_device_info *v4l2_info)
{
    int ret = 0;
    int index = 0;
    int fd = v4l2_info->video_device_fd;

    struct v4l2_input input;
    bzero(&input, sizeof(struct v4l2_input));

    if ((ret = ioctl(fd, VIDIOC_S_INPUT, &index)) == -1) {
        perror("ioctl VIDIOC_S_INPUT");
        dmd_log(LOG_ERR, "ioctl VIDIOC_S_INPUT error:\n", strerror(errno));
        return ret;
    }

    input.index = index;
    if ((ret = ioctl(fd, VIDIOC_ENUMINPUT, &input)) == -1) {
        dmd_log(LOG_ERR, "ioctl VIDIOC_ENUMINPUT error:\n", strerror(errno));
        return ret;
    }

    dmd_log(LOG_INFO, "\n**********input informations**********\n");
    dmd_log(LOG_INFO, "index of the input:%d\n", input.index);
    dmd_log(LOG_INFO, "name of the input:%s\n", input.name);

    return ret;
}


/*
 *	FORMAT   ENUMERATION
 *
 * struct v4l2_fmtdesc {
 * 	__u32		    index;             // Format number      
 * 	enum v4l2_buf_type  type;              // buffer type        
 * 	__u32               flags;
 * 	__u8		    description[32];   // Description string 
 * 	__u32		    pixelformat;       // Format fourcc      
 * 	__u32		    reserved[4];
 * };
 *
 * traverse video stream format, query video format this video device support.
 */
int video_fmtdesc(struct v4l2_device_info *v4l2_info)
{
    int ret = 0;
    int fd = v4l2_info->video_device_fd;
    struct v4l2_fmtdesc fmtdesc;
    bzero(&fmtdesc, sizeof(struct v4l2_fmtdesc));

    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    dmd_log(LOG_INFO, "\n*vidioc enumeration stream format informations*\n");
    while (1) {
        if ((ret = ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc)) == -1) {
            if (errno == EINVAL) { // take it as normal exit
                ret = 0;
                break;
            } else {
                dmd_log(LOG_ERR, "ioctl VIDIOC_ENUM_FMT error:\n",
                        strerror(errno));
                break;
            }
        }

        dmd_log(LOG_INFO, "pixel format = %c%c%c%c, description = %s\n",
                (fmtdesc.pixelformat & 0xFF),
                ((fmtdesc.pixelformat >> 8) & 0xFF),
                ((fmtdesc.pixelformat >> 16) & 0xFF),
                ((fmtdesc.pixelformat >> 24) & 0xFF),
                fmtdesc.description);

        if (fmtdesc.type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
            dmd_log(LOG_INFO, "video capture type:");
        }
        if (fmtdesc.pixelformat == V4L2_PIX_FMT_YUYV) {
            dmd_log(LOG_INFO, "V4L2_PIX_FMT_YUYV\n");
        }

        fmtdesc.index++;
    }

    return ret;
}

/**
 * struct v4l2_format - stream data format
 * @type:	type of the data stream
 * @pix:	definition of an image format
 * @pix_mp:	definition of a multiplanar image format
 * @win:	definition of an overlaid image
 * @vbi:	raw VBI capture or output parameters
 * @sliced:	sliced VBI capture or output parameters
 * @raw_data:	placeholder for future extensions and custom formats
 *
 *
 * struct v4l2_format {
 *     enum v4l2_buf_type type;
 *     union {
 *         struct v4l2_pix_format pix;           // V4L2_BUF_TYPE_VIDEO_CAPTURE
 * 	   struct v4l2_pix_format_mplane pix_mp; // V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE
 *         struct v4l2_window win;               // V4L2_BUF_TYPE_VIDEO_OVERLAY
 *         struct v4l2_vbi_format vbi;           // V4L2_BUF_TYPE_VBI_CAPTURE
 *         struct v4l2_sliced_vbi_format sliced; // V4L2_BUF_TYPE_SLICED_VBI_CAPTURE
 *         __u8 raw_data[200];                   // user-defined
 * 	} fmt;
 * };
 *
 * set video stream data format
 */
int video_setfmt(struct v4l2_device_info *v4l2_info)
{
    int ret = 0;
    int fd = v4l2_info->video_device_fd;
    struct v4l2_format fmt;
    bzero(&fmt, sizeof(struct v4l2_format));

    // stream data format
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = v4l2_info->width;
    fmt.fmt.pix.height = v4l2_info->height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if ((ret = ioctl(fd, VIDIOC_S_FMT, &fmt)) == -1) {
        dmd_log(LOG_ERR, "ioctl VIDIOC_S_FMT error:\n", strerror(errno));
        return ret;
    } else {
        dmd_log(LOG_INFO,
                "set video stream data format to YUYV succeed!\n");
    }

    return ret;
}
// query video data format
int video_getfmt(struct v4l2_device_info *v4l2_info)
{
    int ret = 0;
    int fd = v4l2_info->video_device_fd;
    struct v4l2_format fmt;
    bzero(&fmt, sizeof(struct v4l2_format));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if ((ret = ioctl(fd, VIDIOC_G_FMT, &fmt)) == -1) {
        dmd_log(LOG_ERR, "ioctl VIDIOC_G_FMT error:\n", strerror(errno));
        return ret;
    }

    dmd_log(LOG_INFO, "\n****vidioc get stream format informations****\n");
    if (fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV) {
        dmd_log(LOG_INFO, "8-bit YUYV pixel format.\n");
    }
    dmd_log(LOG_INFO, "Size of the buffer = %d\n", fmt.fmt.pix.sizeimage);
    dmd_log(LOG_INFO, "Line offset = %d\n", fmt.fmt.pix.bytesperline);
    if (fmt.fmt.pix.field == V4L2_FIELD_INTERLACED) {
    	dmd_log(LOG_INFO, "Storate format is interlaced frame format\n");
    }

    return ret;
}

/*
 *	MEMORY-MAPPING BUFFERS
 *
 * struct v4l2_requestbuffers {
 * 	__u32			count;
 * 	enum v4l2_buf_type      type;
 * 	enum v4l2_memory        memory;
 * 	__u32			reserved[2];
 * };
 *
 * struct v4l2_buffer - video buffer info
 * @index:	id number of the buffer
 * @type:	buffer type (type == *_MPLANE for multiplanar buffers)
 * @bytesused:	number of bytes occupied by data in the buffer (payload);
 *		unused (set to 0) for multiplanar buffers
 * @flags:	buffer informational flags
 * @field:	field order of the image in the buffer
 * @timestamp:	frame timestamp
 * @timecode:	frame timecode
 * @sequence:	sequence count of this frame
 * @memory:	the method, in which the actual video data is passed
 * @offset:	for non-multiplanar buffers with memory == V4L2_MEMORY_MMAP;
 *		offset from the start of the device memory for this plane,
 *		(or a "cookie" that should be passed to mmap() as offset)
 * @userptr:	for non-multiplanar buffers with memory == V4L2_MEMORY_USERPTR;
 *		a userspace pointer pointing to this buffer
 * @planes:	for multiplanar buffers; userspace pointer to the array of plane
 *		info structs for this buffer
 * @length:	size in bytes of the buffer (NOT its payload) for single-plane
 *		buffers (when type != *_MPLANE); number of elements in the
 *		planes array for multi-plane buffers
 * @input:	input number from which the video data has has been captured
 *
 * Contains data exchanged by application and driver using one of the Streaming
 * I/O methods.
 *
 * struct v4l2_buffer {
 * 	__u32			index;
 * 	enum v4l2_buf_type      type;
 * 	__u32			bytesused;
 * 	__u32			flags;
 * 	enum v4l2_field		field;
 * 	struct timeval		timestamp;
 * 	struct v4l2_timecode	timecode;
 * 	__u32			sequence;
 * 
 * 	// memory location
 * 	enum v4l2_memory        memory;
 * 	union {
 * 		__u32           offset;
 * 		unsigned long   userptr;
 * 		struct v4l2_plane *planes;
 * 	} m;
 * 	__u32			length;
 * 	__u32			input;
 * 	__u32			reserved;
 * };
 *
 * memory map for the request buffer
 */
int video_mmap(struct v4l2_device_info *v4l2_info)
{
    // step 1, requestbuffers allocating memory
    int ret = 0;
    int fd = v4l2_info->video_device_fd;

    struct v4l2_requestbuffers req;
    bzero(&req, sizeof(struct v4l2_requestbuffers));
    req.count = v4l2_info->reqbuffer_count;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if ((ret = ioctl(fd, VIDIOC_REQBUFS, &req)) == -1) {
        dmd_log(LOG_ERR, "ioctl VIDIOC_REQBUFS error:\n", strerror(errno));
        return -1;
    }

    dmd_log(LOG_INFO, "\n**********video mmap**********\n");
    if (req.count < v4l2_info->reqbuffer_count) {
        dmd_log(LOG_ERR, "Insufficient buffer memory\n");
    }
    dmd_log(LOG_INFO, "Number of buffers allocated = %d\n", req.count);

    // step 2, getting physical address
    int n_buffer = req.count;
    struct mmap_buffer *buffers = v4l2_info->buffers;
    assert(buffers != NULL);
    for (n_buffer = 0; n_buffer < req.count; n_buffer++) {
        struct v4l2_buffer buf; // stand for a frame in driver
        bzero(&buf, sizeof(struct v4l2_buffer));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffer;

        if((ret = ioctl(fd, VIDIOC_QUERYBUF, &buf)) == -1) {
            dmd_log(LOG_ERR, "ioctl VIDIOC_QUERYBUF error:\n", strerror(errno));
            return ret;
        }

        // step 3, Mapping kernel space address to user space
        buffers[n_buffer].length = buf.length;
        buffers[n_buffer].start = mmap(NULL, buf.length,
                PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

        if (buffers[n_buffer].start == MAP_FAILED) {
            ret = -1;
            dmd_log(LOG_ERR, "mmap failed.\n");
            return ret;
        }
    }

    return ret;
}
