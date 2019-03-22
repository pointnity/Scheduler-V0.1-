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
	}

	while (getline(&line, &len, f) != -1) {
		err = callback(line, data);
		if (err)
			break;
	}

	if (line)
		free(line);
	fclose(f);
	return err;
}

int lxc_char_left_gc(char *buffer, size_t len)
{
	int i;
	for (i = 0; i < len; i++) {
		if (buffer[i] == ' ' ||
		    buffer[i] == '\t')
			continue;
		return i;
	}
	return 0;
}

int lxc_char_right_gc(char *buffer, size_t len)
{
	int i;
	for (i = len - 1; i >= 0; i--) {
		if (buffer[i] == ' '  ||
		    buffer[i] == '\t' ||
		    buffer[i] == '\n' ||
		    buffer[i] == '\0')
			continue;
		return i + 1;
	}
	return 0;
}

int lxc_is_line_empty(char *line)
{
	int i;
	size_t len = strlen(line);

	for (i = 0; i < len; i++)
		if (line[i] != ' ' && line[i] != '\t' &&
		    line[i] != '\n' && line[i] != '\r' &&
		    line[i] != '\f' && line[i] != '\0')
			return 0;
	return 1;
}
