/*
 *  Copyright (C) 2012 Simon Busch <morphis@gravedo.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <glib.h>
#include <pthread.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include <libsuspend.h>

#include "resume_handler.h"

#define WAKEUP_SOURCE_PATH		"/tmp/wakeup_source"

static GMainLoop *mainloop = NULL;
static int resume_handlers_not_active = 0;

void signal_handler(int signal)
{
	g_main_loop_quit(mainloop);
}

static gboolean suspend_system_cb(gpointer user_data)
{
	g_message("Going to suspend the system now ...");

	libsuspend_prepare_suspend();

	libsuspend_enter_suspend();

	return FALSE;
}

void wakeup_system(const char *reason, const char *wakelock_to_release)
{
	libsuspend_exit_suspend();

	if (wakelock_to_release)
		libsuspend_release_wake_lock(wakelock_to_release);

	g_message("Waking up the system ...");

	g_file_set_contents(WAKEUP_SOURCE_PATH, reason, strlen(reason), NULL);

	g_main_loop_quit(mainloop);
}

int main(int argc, char **argv)
{
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);

	if (!g_thread_supported ())
		g_thread_init (NULL);

	libsuspend_init(0);

	mainloop = g_main_loop_new(NULL, FALSE);

	if (power_key_resume_handler_init() < 0) {
		g_warning("Failed to initialize power key resume handler!");
		return -1;
	}

	if (rtc_resume_handler_init() < 0) {
		g_warning("Failed to initialize rtc resume handler!");
		return -1;
	}

	g_timeout_add(100, suspend_system_cb, NULL);

	g_main_loop_run(mainloop);

	rtc_resume_handler_release();
	power_key_resume_handler_release();

	g_main_loop_unref(mainloop);

	return 0;
}

// vim:ts=4:sw=4:noexpandtab
