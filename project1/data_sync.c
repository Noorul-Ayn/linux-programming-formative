/*
 * data_sync.c
 * A simple file synchronization tool.
 * Copies every regular file from a source directory to a
 * destination directory if the source copy is newer than the
 * destination copy, or if the destination copy does not exist.
 *
 * Usage: ./data_sync <source_dir> <destination_dir>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define BUF_SIZE 4096
#define PATH_MAX_LEN 1024

/**
 * copy_file - copies one file from src_path to dst_path
 * Returns 0 on success, -1 on failure.
 */
static int copy_file(const char *src_path, const char *dst_path)
{
	int src_fd, dst_fd;
	ssize_t bytes_read;
	char buffer[BUF_SIZE];

	src_fd = open(src_path, O_RDONLY);
	if (src_fd < 0)
	{
		perror("open source");
		return (-1);
	}

	dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (dst_fd < 0)
	{
		perror("open destination");
		close(src_fd);
		return (-1);
	}

	while ((bytes_read = read(src_fd, buffer, BUF_SIZE)) > 0)
	{
		if (write(dst_fd, buffer, bytes_read) != bytes_read)
		{
			perror("write");
			close(src_fd);
			close(dst_fd);
			return (-1);
		}
	}

	close(src_fd);
	close(dst_fd);
	return (0);
}

/**
 * needs_sync - checks whether a file should be copied
 * Returns 1 if the destination is missing or older than the source.
 */
static int needs_sync(const char *src_path, const char *dst_path)
{
	struct stat src_st, dst_st;

	if (stat(src_path, &src_st) < 0)
		return (0);
	if (stat(dst_path, &dst_st) < 0)
		return (1);
	return (src_st.st_mtime > dst_st.st_mtime);
}

int main(int argc, char **argv)
{
	DIR *dir;
	struct dirent *entry;
	struct stat st;
	char src_path[PATH_MAX_LEN], dst_path[PATH_MAX_LEN];
	int synced = 0, skipped = 0;
	time_t now;

	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s <source_dir> <destination_dir>\n",
			argv[0]);
		return (EXIT_FAILURE);
	}

	dir = opendir(argv[1]);
	if (dir == NULL)
	{
		perror("opendir");
		return (EXIT_FAILURE);
	}

	mkdir(argv[2], 0755);

	while ((entry = readdir(dir)) != NULL)
	{
		snprintf(src_path, sizeof(src_path), "%s/%s",
			 argv[1], entry->d_name);
		snprintf(dst_path, sizeof(dst_path), "%s/%s",
			 argv[2], entry->d_name);

		if (stat(src_path, &st) < 0 || !S_ISREG(st.st_mode))
			continue;

		if (needs_sync(src_path, dst_path))
		{
			if (copy_file(src_path, dst_path) == 0)
			{
				printf("Synced: %s\n", entry->d_name);
				synced++;
			}
		}
		else
		{
			skipped++;
		}
	}

	closedir(dir);

	now = time(NULL);
	printf("Sync complete at %s", ctime(&now));
	printf("Files synced: %d, files skipped: %d\n", synced, skipped);

	return (EXIT_SUCCESS);
}
