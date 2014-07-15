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
 * File: parser.c
 *
 * Brief: parse the config file.
 *
 * Date: 2014.07.10
 *
 * Author: weizhenwei <weizhenwei1988@gmail.com>
 *
 * *****************************************************************************
 */

#include "parser.h"


struct config *new_config(const char comment_char,
        const char separator_char)
{
    struct config *conf = (struct config *)malloc(sizeof(struct config));
    if (conf == NULL) {
        dmd_log(LOG_ERR, "in function %s, malloc for struct config failed\n",
                __func__);
        return NULL;
    }
    bzero(conf, sizeof(struct config));

    conf->comment_char = comment_char;
    conf->separator_char = separator_char;
    conf->total_item = 0;
    conf->items = NULL;
    conf->tail = NULL;

    // heading node is not used;
    struct config_item *item = (struct config_item *)
        malloc(sizeof(struct config_item));
    if (item == NULL) {
        dmd_log(LOG_ERR, "in function %s, malloc for config_item failed\n",
                __func__);
        free(conf);
        return NULL;
    } else {
        item->key = NULL;
        item->value = NULL;
        conf->items = item;
        conf->tail = item;
    }

    return conf;
}

static int add_config_item(struct config *conf,
        const char *key, const char *value)
{
    struct config_item *item = (struct config_item *)
        malloc(sizeof(struct config_item));
    if (item == NULL) {
        dmd_log(LOG_ERR, "in function %s, malloc for config_item failed\n",
                __func__);
        return -1;
    } else {
        item->key = strdup(key);
        item->value = strdup(value);
        item->next = NULL;

        conf->tail->next = item;
        conf->tail = conf->tail->next;
        conf->total_item++;
    }

    return 0;
}

int parse_config_file(const char *config_file, struct config *conf)
{
    int ret = -1;

    FILE *fp = fopen(config_file, "r");
#define LINE 1024
    char buffer[LINE];
    if (fp == NULL) {
        dmd_log(LOG_ERR, "in function %s, "
                "can not open config file %s for read\n", config_file);
        return -1;
    }

    // TODO: we assume that the chars of each line is less than 1024
    // remove this assumption later!
    char *p = fgets(buffer, LINE, fp);
    while ( p != NULL) {

        dmd_log(LOG_DEBUG, "in function %s, config line is %s",
                __func__, buffer);
        if (buffer[0] == conf->comment_char) { // comment line;
            p = fgets(buffer, LINE, fp);
            continue;
        }
        if (buffer[0] == '\n') { // empty line;
            p = fgets(buffer, LINE, fp);
            continue;
        }

        // judge line's correctness;
        char *sep = strchr(buffer, conf->separator_char);
        if (sep == NULL) {
            dmd_log(LOG_ERR, "first judge, bad config line:%s\n", buffer);
            return -1;
        }
        char *end = strchr(sep + 1, conf->separator_char);
        if (end != NULL) {
            dmd_log(LOG_ERR, "second judge, bad config line:%s\n", buffer);
            return -1;
        }

        // find key and value;
        char key[LINE];
        char value[LINE];
        int len = strlen(buffer);
        strncpy(key, buffer, sep - buffer);
        key[sep - buffer] = '\0';
        // warning: remember to remove the tail '\n';
        strncpy(value, sep + 1, buffer + len - (sep + 1) - 1);
        value[buffer + len - (sep + 1) - 1] = '\0';
        dmd_log(LOG_INFO, "key = %s, value = %s, value len = %d\n",
                key, value, strlen(value));

        // add config_item to config;
        ret = add_config_item(conf, key ,value);
        assert(ret == 0);

        p = fgets(buffer, LINE, fp);
    }

    fclose(fp);

#undef LINE

    dump_config(conf);

    return 0;
}

void dump_config(const struct config *conf)
{
    int total_item = conf->total_item;
    int counter = 0;
    struct config_item *item = conf->items;

    dmd_log(LOG_DEBUG, "in function %s, "
            "there total %d config items\n", __func__, total_item);

    // heading node is not used;
    while (item->next != NULL) {
        item = item->next;
        counter++;

        dmd_log(LOG_DEBUG, "item info: key = %s, value = %s\n",
                item->key, item->value);
    }

    assert(counter == total_item);
}

void release_config(struct config *conf)
{
    if (conf == NULL) {
        return;
    }

    struct config_item *item = conf->items;
    struct config_item *p = item;
    while (item != NULL) {
        p = item;
        item = item->next;
        if (p->key != NULL) {
            free(p->key);
            p->key = NULL;
        }
        if (p->value != NULL) {
            free(p->value);
            p->value = NULL;
        }
        free(p);
    }
    
    free(conf);
}
