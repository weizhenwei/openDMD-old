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
 * File: statistics.h
 *
 * Brief: motion detection statistics while opendmd is running;
 *
 * Date: 2014.07.31
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#ifndef STATISTICS_H
#define STATISTICS_H

#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>

#include "global_context.h"
#include "log.h"

struct motion_t {
    // public
    time_t start;  // start time of motion;
    time_t end;    // end time of motion;
    uint64_t pictures; // for picture_thread use;
    uint64_t video_frames; // for video_thread use;

    // private
    char *video_path;      // for showing video path?

    struct motion_t *next; // for next node;
};

struct stats {
    pthread_mutex_t mutex;    // for mutiple thread synchronization;
    struct motion_t *motion_list;
    struct motion_t *current_motion;
    uint32_t num_motions;
    uint64_t total_pictures; // for picture_thread statistics;
    uint64_t total_video_frames; // for video_thread statistics;
};

// global statistics variable;
extern struct stats *global_stats;

// struct motion_t operations;
extern struct motion_t *new_motion();
extern void set_motion_start_time(struct motion_t *motion,
        const time_t start_time);
extern void set_motion_end_time(struct motion_t *motion,
        const time_t end_time);
extern void increase_motion_pictures(struct motion_t *motion);
extern void increase_motion_video_frames(struct motion_t *motion);
extern void set_motion_videopath(struct motion_t *motion,
        const char *video_path);
// add struct motion_t motion to struct stats stats
extern int add_motion(struct stats *stats, struct motion_t *motion);


extern struct stats *new_statistics();
// at the end of programming running, dump the statistics;
extern void dump_statistics(const struct stats *stats);

// release memory
extern void release_statistics(struct stats *stats);


#endif
