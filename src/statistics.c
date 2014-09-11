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
 * File: statistics.c
 *
 * Brief: motion detection statistics while opendmd is running;
 *
 * Date: 2014.07.31
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "src/statistics.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "src/global_context.h"
#include "src/log.h"

#if 0
struct stats global_stats = {
    .motion_list = NULL,
    .num_motions = 0,
    .total_pictures = 0,
    .total_video_frames = 0
};
#endif

// global statistics variable;
struct stats *global_stats = NULL;

struct stats *new_statistics() {
    struct stats *stats = (struct stats *)malloc(sizeof(struct stats));
    assert(stats != NULL);
    bzero(stats, sizeof(stats));

    pthread_mutex_init(&stats->mutex, NULL);
    stats->motion_list = NULL;
    stats->current_motion = NULL;
    stats->num_motions = 0;
    stats->total_pictures = 0;
    stats->total_video_frames = 0;

    return stats;
}

// struct motion_t operations;
// create a new struct motion_t
struct motion_t *new_motion() {
    struct motion_t *motion = (struct motion_t *)
        malloc(sizeof(struct motion_t));
    assert(motion != NULL);
    bzero(motion, sizeof(struct motion_t));

    motion->start = 0;
    motion->end = 0;
    motion->pictures = 0;
    motion->video_frames = 0;
    // when calling strdup, remember to free later!
    motion->video_path = NULL;
    motion->next = NULL;

    return motion;
}
void set_motion_start_time(struct motion_t *motion, const time_t start_time) {
    motion->start = start_time;
}
void set_motion_end_time(struct motion_t *motion, const time_t end_time) {
    motion->end = end_time;
}
void set_motion_duration(struct motion_t *motion) {
    motion->duration = motion->end - motion->start;
    assert(motion->duration >= 0);
}

void increase_motion_pictures(struct motion_t *motion) {
    motion->pictures++;
}
void increase_motion_video_frames(struct motion_t *motion) {
    motion->video_frames++;
}
void set_motion_videopath(struct motion_t *motion, const char *video_path) {
    // WARNING: when calling strdup, remember to free later!
    motion->video_path = strdup(video_path);
    assert(motion->video_path != NULL);
}

// add struct motion_t motion to struct stats stats
int add_motion(struct stats *stats, struct motion_t *motion) {
    assert(stats != NULL);
    assert(motion != NULL);

    if (stats->motion_list == NULL) {  // motion is the first;
        stats->motion_list = motion;
        stats->num_motions = 1;
        stats->total_pictures = motion->pictures;
        stats->total_video_frames = motion->video_frames;
    } else {  // insert motion at head of stats->motion_list;
        motion->next = stats->motion_list;
        stats->motion_list = motion;
        stats->num_motions += 1;
        stats->total_pictures += motion->pictures;
        stats->total_video_frames += motion->video_frames;
    }

    return 0;
}

static void dump_motion(const struct motion_t *motion) {
    dmd_log(LOG_INFO, "motion start time:%s",
            ctime((const time_t *) &motion->start));
    dmd_log(LOG_INFO, "motion end time:%s",
            ctime((const time_t *) &motion->end));
    dmd_log(LOG_INFO, "motion duration time:%d seconds\n", motion->duration);
    dmd_log(LOG_INFO, "motion with %d pictures captured\n",
            motion->pictures);
    dmd_log(LOG_INFO, "motion with %d video frames captured\n",
            motion->video_frames);
    if (motion->video_path != NULL) {
        dmd_log(LOG_INFO, "motion video path is:%s\n",
                motion->video_path);
    }

    return;
}

void dump_statistics(const struct stats *stats) {
    dmd_log(LOG_INFO, "****************************AT END"
            "****************************\n");
    dmd_log(LOG_INFO, "DUMP the motion detection statistics "
            "while opendmd was running:\n");
    dmd_log(LOG_INFO, "Detected %d motions.\n", stats->num_motions);
    dmd_log(LOG_INFO, "Captured total %d pictures.\n", stats->total_pictures);
    dmd_log(LOG_INFO, "Captured total %d video frames.\n",
            stats->total_video_frames);

    if (stats->num_motions > 0) {
        dmd_log(LOG_INFO, "Each motion displays as following:\n");

        int i = 0;
        struct motion_t *m = stats->motion_list;
        while (m != NULL) {
            i++;
            dmd_log(LOG_INFO, "Dump motion %d:\n", i);
            dump_motion(m);
            m = m->next;
        }
        assert(i == stats->num_motions);
    }
}

// release memory
void release_statistics(struct stats *stats) {
    struct motion_t *motion = stats->motion_list;
    struct motion_t *m = motion;
    while (motion != NULL) {
        m = motion;
        motion = motion->next;

        if (m->video_path != NULL) {
            free(m->video_path);
            m->video_path = NULL;
        }

        free(m);
        m = NULL;
    }
    if (stats->current_motion != NULL) {
        if (stats->current_motion->video_path != NULL) {
            free(stats->current_motion->video_path);
            stats->current_motion->video_path = NULL;
        }

        free(stats->current_motion);
    }

    pthread_mutex_destroy(&stats->mutex);

    free(stats);
}

