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
 * Brief: file path operation for storing picture and video. 
 *
 * Date: 2014.07.31
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "statistics.h"

struct stats global_stats = {
    .motion_list = NULL,
    .num_motions = 0,
    .total_pictures = 0,
    .total_video_frames = 0
};

// add struct motion_t motion to struct stats stats
int add_motion(struct stats *stats, struct motion_t *motion)
{
    assert(stats != NULL);
    assert(motion != NULL);

    if (stats->motion_list == NULL) { // motion is the first;
        stats->motion_list = motion;
        stats->num_motions = 1;
        stats->total_pictures = motion->pictures;
        stats->total_video_frames = motion->video_frames;
    } else { // insert motion at head of stats->motion_list;
        motion->next = stats->motion_list;
        stats->motion_list = motion;
        stats->num_motions += 1;
        stats->total_pictures += motion->pictures;
        stats->total_video_frames += motion->video_frames;
    }

    return 0;
}

static void dump_motion(const struct motion_t *motion)
{

    // TODO: fulfill this function later!
    return ;
}

void dump_statistics(const struct stats *stats)
{
    // TODO: fulfill this function later!
    dmd_log(LOG_INFO, "in function %s, Detected %d motions:\n",
            __func__, stats->num_motions);
}


