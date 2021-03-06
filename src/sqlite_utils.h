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

#ifndef SRC_SQLITE_UTILS_H_
#define SRC_SQLITE_UTILS_H_

#include <sqlite3.h>

#define DEFAULT_DATABASE "opendmd.db"
#define DEFAULT_TABLE "opendmd_table"

// this two struct shold be declared first,
// or the gcc warning "struct XX declared inside parameter list"
// will occur.
struct motion_t;
struct stats;

extern char database_file[];
extern sqlite3 *opendmd_db;

extern int init_database();

extern sqlite3 *open_db(const char *database);

extern int exec_SQL(sqlite3 *db, const char *sql,
        int (*callback)(void *, int, char **, char **), void *firstarg);

extern int create_table(sqlite3 *db, const char *table_name);

extern int insert_item(sqlite3 *db, const char *table_name,
        const struct motion_t *motion);

extern int store_motion_to_database(const struct stats *stats);

extern int dump_database_table(sqlite3 *db, const char *table_name);

extern int dump_database_table_to_fd(sqlite3 *db,
        const char *table_name, int fd);

extern int clean_database_table(sqlite3 *db, const char *table_name);

extern int close_db(sqlite3 *db);

#endif  // SRC_SQLITE_UTILS_H_

