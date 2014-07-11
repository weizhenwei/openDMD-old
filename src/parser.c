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
    }
    bzero(conf, sizeof(struct config));

    conf->comment_char = comment_char;
    conf->separator_char = separator_char;
    conf->total_item = 0;
    conf->items = NULL;

    return conf;
}

int parse_config_file(const char *config_file, struct config *conf)
{
    dmd_log(LOG_INFO, "hehe\n");
    FILE *fp = fopen(config_file, "r");
#define LINE 1024
    char buffer[LINE];
    if (fp == NULL) {
        dmd_log(LOG_ERR, "in function %s, "
                "can not open config file %s for read\n", config_file);
        return -1;
    }

    char *p = fgets(buffer, LINE, fp);
    while ( p != NULL) {
        printf("%s\n", buffer);
    }

    fclose(fp);

    return 0;
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
