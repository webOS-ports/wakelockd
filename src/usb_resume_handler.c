/*
 *  Copyright (C) 2014 Simon Busch <morphis@gravedo.de>
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
#include <stdlib.h>
#include <errno.h>

#include <glib.h>
#include <lunaservice.h>
#include <pbnjson.h>
#include <libsuspend.h>

#include "resume_handler.h"

extern LSHandle *service_handle;

static bool usb_dock_status_changed_cb(LSHandle *handle, LSMessage *message, void *user_data)
{
	const char *payload;
	jschema_ref input_schema = NULL;
	jvalue_ref parsed_obj = NULL;
	jvalue_ref usb_connected_obj = NULL;
	JSchemaInfo schema_info;
	bool usb_connected = false;

	libsuspend_acquire_wake_lock("wakelockd_usb_event");

	payload = LSMessageGetPayload(message);
	if (!payload)
		goto done;

	input_schema = jschema_parse(j_cstr_to_buffer("{}"), DOMOPT_NOOPT, NULL);
	jschema_info_init(&schema_info, input_schema, NULL, NULL);

	parsed_obj = jdom_parse(j_cstr_to_buffer(payload), DOMOPT_NOOPT, &schema_info);

	jschema_release(&input_schema);
	if (jis_null(parsed_obj))
		goto done;

	if (!jobject_get_exists(parsed_obj, J_CSTR_TO_BUF("USBConnected"), &usb_connected_obj) ||
		!jis_boolean(usb_connected_obj))
		goto done;

	jboolean_get(usb_connected_obj, &usb_connected);

	if (usb_connected)
		wakeup_system("usb", "wakelockd_usb_event");

done:
	libsuspend_release_wake_lock("wakelockd_usb_event");

	return true;
}

int usb_resume_handler_init(void)
{
	LSError lserror;

	LSErrorInit(&lserror);

	if (!LSCall(service_handle, "palm://com.palm.lunabus/signal/addmatch",
				"{\"category\":\"/com/palm/power\",\"method\":\"USBDockStatus\"}",
				usb_dock_status_changed_cb, NULL, NULL, &lserror)) {
		g_message("Could not register match for USB dock status signal: %s", lserror.message);
		LSErrorFree(&lserror);
		return -1;
	}

	return 0;
}

void usb_resume_handler_release(void)
{
}

// vim:ts=4:sw=4:noexpandtab
