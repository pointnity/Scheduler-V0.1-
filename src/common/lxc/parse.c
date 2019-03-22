/*
 * lxc: linux Container library
 */
#define _GNU_SOURCE
#include <stdio.h>
#undef _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>

#include "parse.h"
#include <lxc/log.h>

lxc_log_define(lxc_parse, lxc);

static int dir_filter(const struct dirent *dirent)
{
	if (!strcmp(dirent->d_name, ".") ||
            !strcmp(dirent->d_name, ".."))
                return 0;
        return 1;
}

int lxc_dir_for_each(const char *name, const char *directory,
		     lxc_dir_cb callback, void *data)
{
	struct dirent **namelist;
	int n, ret = 0;

	n = scandir(directory, &namelist, dir_filter, alphasort);
	if (n < 0) {
		SYSERROR("failed to scan %s directory", directory);
		return -1;
	}

	while (n--) {
		if (!ret &&
		    callback(name, directory, namelist[n]->d_name, data)) {
			ERROR("callback failed");
			ret = -1;
		}
		free(namelist[n]);
	}
	free(namelist);

	return ret;
}

int lxc_file_for_each_line(const char *file, lxc_file_cb callback, void *data)
{
	FILE *f;
	int err = 0;
	char *line = NULL;
	size_t len = 0;

	f = fopen(file, "r");
	if (!f) {
		SYSERROR("failed to open %s", file);
		return -1;
