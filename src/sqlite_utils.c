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
 * File: sqlite_utils.c
 *
 * Brief: sqlite manipulation interface utils;
 *
 * Date: 2014.08.15
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "sqlite_utils.h"

// global database file;
char database_file[PATH_MAX];
sqlite3 *opendmd_db = NULL;

int init_database()
{
    int ret = test_and_mkdir(global.database_repo);
    assert(ret == 0);

    // open/create default database opendmd.db
    sprintf(database_file, "%s/%s", global.database_repo, DEFAULT_DATABASE);
    dmd_log(LOG_DEBUG, "database_file is %s\n", database_file);
    opendmd_db = open_db(database_file);
    assert(opendmd_db != NULL);

    // create default table opendmd_table
    int rc = create_table(opendmd_db, DEFAULT_TABLE);
    assert(rc == 0);

    return 0;
}

sqlite3 *open_db(const char *database)
{
    // define the sqlite data connection object;
    sqlite3 *db = NULL;

    int rc = sqlite3_open(database, &db);
    if (rc != SQLITE_OK) {
        dmd_log(LOG_ERR, "can't open the database:%s\n", sqlite3_errmsg(db));
        return NULL;
    }
    dmd_log(LOG_DEBUG, "connect database sucess!\n");

    return db;
}


// TODO: this needs further refactor;
int exec_SQL(sqlite3 *db, const char *sql)
{
    char *errmsg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        dmd_log(LOG_ERR, "execute sql \"%s\" error:%s\n", sql, errmsg);
        return -1;
    } else {
        dmd_log(LOG_DEBUG, "execute sql \"%s\" success\n", sql);
        return 0;
    }

    return 0;
}

int create_table(sqlite3 *db, const char *table_name)
{
    char create_table_sql[PATH_MAX];
    // creat table if not exists, "is not exists" is important!
    sprintf(create_table_sql, "CREATE TABLE IF NOT EXISTS %s "
            "(start_time TEXT PRIMARY KEY, end_time TEXT, duration INT, "
            "pictures INT, video_frames INT, video_path TEXT)", table_name);

    char *errmsg = NULL;
    int rc = sqlite3_exec(db, create_table_sql, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        dmd_log(LOG_ERR, "execute sql \"%s\" error:%s\n",
                create_table_sql, errmsg);
        return -1;
    } else {
        dmd_log(LOG_DEBUG, "execute sql \"%s\" success\n", create_table_sql);
        return 0;
    }

    return 0;
}

int insert_item(sqlite3 *db, const char *table_name,
        struct add_motion_sql_clause *add_motion)
{
    assert(add_motion != NULL);
    char insert_item_sql[PATH_MAX];
    sprintf(insert_item_sql, "INSERT INTO %s VALUES(%s, %s, %ld, %ld, %ld, %s)",
            table_name, ctime(&add_motion->start_time),
            ctime(&add_motion->end_time), add_motion->duration,
            add_motion->pictures, add_motion->video_frames,
            add_motion->video_path);

    char *errmsg = NULL;
    int rc = sqlite3_exec(db, insert_item_sql, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        dmd_log(LOG_ERR, "execute sql \"%s\" error:%s\n",
                insert_item_sql, errmsg);
        return -1;
    } else {
        dmd_log(LOG_DEBUG, "execute sql \"%s\" success\n", insert_item_sql);
        return 0;
    }

    return 0;
}

int close_db(sqlite3 *db)
{
    int rc = sqlite3_close(db);
    if (rc != SQLITE_OK) {
        dmd_log(LOG_ERR, "can't close the database:%s\n",
                sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}
