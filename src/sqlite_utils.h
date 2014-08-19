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
 * File: sqlite_utils.h
 *
 * Brief: sqlite manipulation interface utils;
 *
 * Date: 2014.08.15
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#ifndef SQLITE_UTILS_H
#define SQLITE_UTILS_H

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <linux/limits.h>

#include <sqlite3.h>

#include "log.h"
#include "path.h"

#define DEFAULT_DATABASE "opendmd.db"
#define DEFAULT_TABLE "opendmd_table"


// add an detected motion to sqlite database;
struct add_motion_sql_clause {
    time_t start_time;          // motion start time;
    time_t end_time;            // motion end time;
    uint64_t duration;          // motion duration time;
    uint64_t pictures;          // pictures in this motion, if any;
    uint64_t video_frames;      // total video frames in this motion;
    char video_path[PATH_MAX];  // video file storage path;
};

extern char database_file[];
extern sqlite3 *opendmd_db;

extern int init_database();

extern sqlite3 *open_db(const char *database);

extern int exec_SQL(sqlite3 *db, const char *sql);

extern int create_table(sqlite3 *db, const char *table_name);

extern int insert_item(sqlite3 *db, const char *table_name,
        struct add_motion_sql_clause *add_motion);

extern int close_db(sqlite3 *db);



#endif
