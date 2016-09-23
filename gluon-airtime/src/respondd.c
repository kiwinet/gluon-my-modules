/*
  Copyright (c) 2016, Andrej Sidorenko <avita@hostega.eu>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <respondd.h>

#include <json-c/json.h>
#include <libgluonutil.h>
#include <libplatforminfo.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <uci.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/utsname.h>
#include <sys/vfs.h>

static struct json_object * airtime2(void) {
        ssize_t r;
        char *line = NULL;
        size_t len = 0;
        double act2;
        double bus2;

        FILE *f = popen("exec iw client0 survey dump |grep 'in use' -A5|grep active|grep -o '[0-9]*'", "r");
        if (f) {
                r = getline(&line, &len, f);
        }
        pclose(f);

        if (r >= 0) {
                len = strlen(line); /* The len given by getline is the buffer size, not the string length */

                if (len && line[len-1] == '\n')
                        line[len-1] = 0;

                act2 = strtod(line, NULL);
                free(line);
                line = NULL;
        }
        else {
                free(line);
                line = NULL;
        }

        FILE *f = popen("exec iw client0 survey dump |grep 'in use' -A5|grep busy|grep -o '[0-9]*'", "r");
        if (f) {
                r = getline(&line, &len, f);
        }
        pclose(f);

        if (r >= 0) {
                len = strlen(line); /* The len given by getline is the buffer size, not the string length */

                if (len && line[len-1] == '\n')
                        line[len-1] = 0;

                bus2 = strtod(line, NULL);
                free(line);
                line = NULL;
        }
        else {
                free(line);
                line = NULL;
        }


        double rez = bus2 / act2;
        return json_object_new_double(rez);
}

static struct json_object * airtime5(void) {
        ssize_t r;
        char *line = NULL;
        size_t len = 0;
        double act5;
        double bus5;

        FILE *f = popen("exec iw client1 survey dump |grep 'in use' -A5|grep active|grep -o '[0-9]*'", "r");
        if (f) {
                r = getline(&line, &len, f);
        }
        pclose(f);

        if (r >= 0) {
                len = strlen(line); /* The len given by getline is the buffer size, not the string length */

                if (len && line[len-1] == '\n')
                        line[len-1] = 0;

                act5 = strtod(line, NULL);
                free(line);
                line = NULL;
        }
        else {
                free(line);
                line = NULL;
        }

        FILE *f = popen("exec iw client1 survey dump |grep 'in use' -A5|grep busy|grep -o '[0-9]*'", "r");
        if (f) {
                r = getline(&line, &len, f);
        }
        pclose(f);

        if (r >= 0) {
                len = strlen(line); /* The len given by getline is the buffer size, not the string length */

                if (len && line[len-1] == '\n')
                        line[len-1] = 0;

                bus5 = strtod(line, NULL);
                free(line);
                line = NULL;
        }
        else {
                free(line);
                line = NULL;
        }

        double rez = bus5 / act5;

        return json_object_new_double(rez);
}

static struct json_object * chan2(void) {
        ssize_t r;
        char *line = NULL;
        size_t len = 0;
        char *chan2;

        FILE *f = popen("exec iw dev client0 info | awk '/channel/ {printf $2;}'", "r");
        if (f) {
                r = getline(&line, &len, f);
        }
        pclose(f);

        if (r >= 0) {
                len = strlen(line); /* The len given by getline is the buffer size, not the string length */

                if (len && line[len-1] == '\n')
                        line[len-1] = 0;

                chan2 = line;
                free(line);
                line = NULL;
        }
        else {
                free(line);
                line = NULL;
        }


        return json_object_new_string(chan2);
}

static struct json_object * chan5(void) {
        ssize_t r;
        char *line = NULL;
        size_t len = 0;
        char *chan5;

        FILE *f = popen("exec iw dev client1 info | awk '/channel/ {printf $2;}'", "r");
        if (f) {
                r = getline(&line, &len, f);
        }
        pclose(f);

        if (r >= 0) {
                len = strlen(line); /* The len given by getline is the buffer size, not the string length */

                if (len && line[len-1] == '\n')
                        line[len-1] = 0;

                chan5 = line;
                free(line);
                line = NULL;
        }
        else {
                free(line);
                line = NULL;
        }


        return json_object_new_string(chan5);
}


static struct json_object * respondd_provider_nodeinfo(void) {
        struct json_object *ret = json_object_new_object();

        struct json_object *wireless = json_object_new_object();
        json_object_object_add(wireless, "airtime2", airtime2());
        json_object_object_add(wireless, "airtime5", airtime5());
        json_object_object_add(wireless, "chan2", chan2());
        json_object_object_add(wireless, "chan5", chan5());

        json_object_object_add(ret, "wireless", wireless);

        return ret;
}


const struct respondd_provider_info respondd_providers[] = {
	{"nodeinfo", respondd_provider_nodeinfo},
	{}
};
