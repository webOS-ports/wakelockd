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
#include <linux/rtc.h>
#include <sys/ioctl.h>

#include <glib.h>

#include "resume_handler.h"

static int rtc_fd = 0;
static GIOChannel *channel = NULL;
static int readwatch = 0;

#define RTC_DEVICE_PATH		"/dev/rtc"

gboolean _handle_rtc_event(GIOChannel *channel, GIOCondition condition, gpointer user_data)
{
	int bytesread;
	unsigned long data;
	int wakeup = 0;
	int ret;
	struct rtc_wkalrm alarm;

	if ((condition  & G_IO_IN) == G_IO_IN) {
		bytesread = read(rtc_fd, &data, sizeof(unsigned long));
		if (bytesread == 0) {
			g_warning("Got some rtc event but could not read anything -> waking up the system!");
			wakeup = 1;
		}
		else if (data & RTC_AF) {
			g_debug("Got rtc event -> waking up the system!");
			wakeup = 1;

			/* We may have to reschedule the event for some seconds in the future to
			 * let other userland processes like sleepd (which should have issued the
			 * alarm) react on the event as well. */

			ret = ioctl(rtc_fd, RTC_WKALM_RD, &alarm);
			if (ret < 0) {
				g_warning("Failed to read RTC alarm from device; not rescheduling alarm!");
				goto wakeup;
			}

			/* rescheduling two seconds in the future should be enough for userland to
			 * wakeup */
			alarm.time.tm_sec += 2;
			alarm.enabled = 1;
			alarm.pending = 1;

			ret = ioctl(rtc_fd, RTC_WKALM_SET, &alarm);
			if (ret < 0) {
				g_warning("Could not reschedule alarm!");
				goto wakeup;
			}
		}
	}

wakeup:
	if (wakeup)
		wakeup_system("rtc");

	return TRUE;
}

int rtc_resume_handler_init(void)
{
	/* NOTE: opening rtc device her exclusivly will let sleepd to fail with all it's rtc
	 * related operations while we're in suspend. The nyx system module is right now
	 * implemented that way that it's only opening the rtc device eclusively when it needs
	 * it. As long as sleepd reacts correctly for a failed nyx method call this should be
	 * no problem. */
	rtc_fd = open(RTC_DEVICE_PATH, O_RDONLY);
	if (rtc_fd < 0)
		return -ENODEV;

	channel = g_io_channel_unix_new(rtc_fd);
	g_io_channel_set_encoding(channel, NULL, NULL);
	readwatch = g_io_add_watch(channel, G_IO_IN | G_IO_HUP | G_IO_NVAL, _handle_rtc_event, NULL);

	return 0;
}

void rtc_resume_handler_release(void)
{
	if (readwatch > 0)
		g_source_remove(readwatch);

	if (channel != NULL)
		g_io_channel_unref(channel);

	if (rtc_fd > 0)
		close(rtc_fd);
}

// vim:ts=4:sw=4:noexpandtab
