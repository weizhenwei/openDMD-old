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
 * File: test_sqlite.c
 *
 * Brief: test sqlite C API
 *
 * Date: 2014.08.12
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sqlite3.h>

static sqlite3 *open_db(const char *database)
{
    // define an sqlite data connection object;
    sqlite3 *db = NULL;

    int rc = sqlite3_open(database, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr,"%s\n",sqlite3_errmsg(db));
        return NULL;
    }
    printf("connect sucess!\n");

    return db;
}

static void close_db(sqlite3 *db)
{
    int rc = sqlite3_close(db);
    if (rc != SQLITE_OK) {
        printf("can't close the database:%s\n", sqlite3_errmsg(db));
        exit(EXIT_FAILURE);
    }
}

static int exec_SQL(sqlite3 *db, const char *sql)
{
    char *errmsg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        printf("execute sql \"%s\" error:%s\n", sql, errmsg);
        return -1;
    } else {
        printf("execute sql \"%s\" success\n", sql);
        return 0;
    }

    return 0;
}

int main(void)
{
    int rc = -1;
    sqlite3 *db = NULL;
    db = open_db("sqlite.db");
    assert(db != NULL);

    char *create_table_SQL = "create table detected_motions "
        "(id text, start_time int, end_time int, video_path text)";

    rc = exec_SQL(db, create_table_SQL);
    assert(rc == 0);

    char *insert_item_SQL = "insert into "
        "detected_motions(id, start_time, end_time, video_path) "
        "values(\"first motion\", 12, 14, "
        "\"/home/wzw/opendmd/video/a.flv\")";

    rc = exec_SQL(db, insert_item_SQL);
    assert(rc == 0);

    close_db(db);
    return 0;
}

