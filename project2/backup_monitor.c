/*
 * backup_monitor.c
 * A small file backup program used to study how a process
 * interacts with the operating system through system calls.
 *
 * The program performs three operations:
 *   1. File creation  - creates a data file and writes content to it
 *   2. Writing logs   - appends a timestamped entry to a log file
 *   3. Reading files  - reads the data file back and prints it
 *
 * Usage: ./backup_monitor
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define BUF_SIZE 256

/**
 * write_log - appends a timestamped message to the log file
 * Returns 0 on success, -1 on failure.
 */
static int write_log(const char *message)
{
	int log_fd;
	char entry[BUF_SIZE];
	time_t now;

	log_fd = open("backup.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (log_fd < 0)
	{
		perror("open log");
		return (-1);
	}

	now = time(NULL);
	snprintf(entry, sizeof(entry), "[%ld] %s\n", (long)now, message);

	if (write(log_fd, entry, strlen(entry)) < 0)
	{
		perror("write log");
		close(log_fd);
		return (-1);
	}

	close(log_fd);
	return (0);
}

int main(void)
{
	int fd;
	ssize_t bytes_read;
	char buffer[BUF_SIZE];
	const char *content = "Backup data: configuration snapshot v1.\n";

	/* 1. File creation */
	fd = open("backup_data.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		perror("create data file");
		return (EXIT_FAILURE);
	}
	if (write(fd, content, strlen(content)) < 0)
	{
		perror("write data file");
		close(fd);
		return (EXIT_FAILURE);
	}
	close(fd);
	write_log("Data file created and written.");

	/* 2. Reading the file back */
	fd = open("backup_data.txt", O_RDONLY);
	if (fd < 0)
	{
		perror("open data file for reading");
		return (EXIT_FAILURE);
	}
	bytes_read = read(fd, buffer, sizeof(buffer) - 1);
	if (bytes_read < 0)
	{
		perror("read data file");
		close(fd);
		return (EXIT_FAILURE);
	}
	buffer[bytes_read] = '\0';
	close(fd);
	write_log("Data file read back successfully.");

	printf("Recovered content: %s", buffer);
	printf("Backup cycle complete. See backup.log for details.\n");

	return (EXIT_SUCCESS);
}
