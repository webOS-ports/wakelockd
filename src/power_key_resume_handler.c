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
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include <glib.h>

#include <linux/input.h>

#include "resume_handler.h"

static int input_source_fd;
static GIOChannel *channel;
static int readwatch;

#define INPUT_DEVICE_PATH		"/dev/input/event2"

gboolean _handle_input_event(GIOChannel *channel, GIOCondition condition, gpointer data)
{
	int bytesread;
	int wakeup = 0;
	struct input_event ev;

	if ((condition  & G_IO_IN) == G_IO_IN) {
		bytesread = read(input_source_fd, &ev, sizeof(struct input_event));
		if (bytesread == 0) {
			g_warning("Got some input event but could not read anything -> waking up the system!");
			wakeup = 1;
		}
		else if (ev.code == KEY_POWER) {
			g_debug("Got power key input event -> waking up the system!");
			wakeup = 1;
		}
		else {
			g_debug("Got some other input event but not the power key -> NOT wakeing up");
		}
	}

	if (wakeup)
		wakeup_system("power_key");

	return TRUE;
}

int power_key_resume_handler_init(void)
{
	input_source_fd = open(INPUT_DEVICE_PATH, O_RDONLY);
	if (input_source_fd < 0)
		return -ENODEV;

	if (ioctl(input_source_fd, EVIOCGRAB, 1) < 0) {
		close(input_source_fd);
		return -EIO;
	}

	channel = g_io_channel_unix_new(input_source_fd);
	g_io_channel_set_encoding(channel, NULL, NULL);
	readwatch = g_io_add_watch(channel, G_IO_IN | G_IO_HUP | G_IO_NVAL, _handle_input_event, NULL);

	return 0;
}

void power_key_resume_handler_release(void)
{
	g_source_remove(readwatch);
	g_io_channel_unref(channel);

	if (ioctl(input_source_fd, EVIOCGRAB, 0) < 0)
		g_error("Could not ungrab input event source");

	close(input_source_fd);
}

// vim:ts=4:sw=4:noexpandtab
