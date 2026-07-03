/*
 * monitor_service.c
 * A background service simulating a server monitoring application.
 * It prints a heartbeat message every 5 seconds and responds to
 * administrative commands delivered through Unix signals:
 *
 *   SIGINT  - graceful shutdown ("Monitor Service shutting down safely.")
 *   SIGUSR1 - status request  ("System status requested by administrator.")
 *   SIGTERM - emergency stop  ("Emergency shutdown signal received.")
 *
 * Handlers are registered with sigaction(). Inside the handlers only
 * async-signal-safe functions are used: write() for output and _exit()
 * for immediate termination. printf() is not async-signal-safe, so the
 * graceful SIGINT path only sets a flag inside the handler and lets the
 * main loop do the printing.
 *
 * Compile: gcc -Wall -Wextra -o monitor_service monitor_service.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

/* Set by the SIGINT handler, checked by the main loop */
static volatile sig_atomic_t keep_running = 1;

/**
 * handle_sigint - requests a graceful shutdown.
 * Only sets a flag; the main loop notices it and exits cleanly.
 */
static void handle_sigint(int signum)
{
	(void)signum;
	keep_running = 0;
}

/**
 * handle_sigusr1 - reports status to the administrator.
 * Uses write() because it is async-signal-safe.
 */
static void handle_sigusr1(int signum)
{
	const char msg[] = "System status requested by administrator.\n";

	(void)signum;
	write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

/**
 * handle_sigterm - emergency shutdown, exits immediately.
 */
static void handle_sigterm(int signum)
{
	const char msg[] = "Emergency shutdown signal received.\n";

	(void)signum;
	write(STDOUT_FILENO, msg, sizeof(msg) - 1);
	_exit(EXIT_SUCCESS);
}

int main(void)
{
	struct sigaction sa_int, sa_usr1, sa_term;

	/* Register the SIGINT handler */
	memset(&sa_int, 0, sizeof(sa_int));
	sa_int.sa_handler = handle_sigint;
	sigemptyset(&sa_int.sa_mask);
	if (sigaction(SIGINT, &sa_int, NULL) < 0)
	{
		perror("sigaction SIGINT");
		return (EXIT_FAILURE);
	}

	/* Register the SIGUSR1 handler */
	memset(&sa_usr1, 0, sizeof(sa_usr1));
	sa_usr1.sa_handler = handle_sigusr1;
	sigemptyset(&sa_usr1.sa_mask);
	if (sigaction(SIGUSR1, &sa_usr1, NULL) < 0)
	{
		perror("sigaction SIGUSR1");
		return (EXIT_FAILURE);
	}

	/* Register the SIGTERM handler */
	memset(&sa_term, 0, sizeof(sa_term));
	sa_term.sa_handler = handle_sigterm;
	sigemptyset(&sa_term.sa_mask);
	if (sigaction(SIGTERM, &sa_term, NULL) < 0)
	{
		perror("sigaction SIGTERM");
		return (EXIT_FAILURE);
	}

	printf("[Monitor Service] Started. PID: %d\n", getpid());
	fflush(stdout);

	while (keep_running)
	{
		printf("[Monitor Service] System running normally...\n");
		fflush(stdout);
		sleep(5);
	}

	printf("Monitor Service shutting down safely.\n");
	return (EXIT_SUCCESS);
}
