#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <errno.h>
#include <string.h>

#include "preempt.h"
#include "uthread.h"

static sigset_t g_sigset;

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

static void preempt_handler(int signo)
{
	uthread_yield();
}

void preempt_disable(void)
{
	sigprocmask(SIG_BLOCK, &g_sigset, NULL);
}

void preempt_enable(void)
{
	sigprocmask(SIG_UNBLOCK, &g_sigset, NULL);
}

void preempt_start(void)
{
	static struct itimerval g_timer;
	static struct sigaction g_sigaction;

	sigemptyset(&g_sigset);
	sigaddset(&g_sigset, SIGALRM);

	sigemptyset(&g_sigaction.sa_mask);
	g_sigaction.sa_flags = 0;
	g_sigaction.sa_handler = &preempt_handler;
	sigaction(SIGALRM, &g_sigaction, NULL);

	g_timer.it_value.tv_sec = 0;
	g_timer.it_value.tv_usec = (int)(1e6 / HZ);
	g_timer.it_interval.tv_sec = 0;
	g_timer.it_interval.tv_usec = (int)(1e6 / HZ);
	setitimer(ITIMER_REAL, &g_timer, NULL);
}