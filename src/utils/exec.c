/*
 * Copyright (C) 2014  Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Vratislav Podzimek <vpodzime@redhat.com>
 */

#include <glib.h>
#include "exec.h"

/**
 * bd_utils_exec_and_report_error:
 * @argv: (array zero-terminated=1): the argv array for the call
 * @error_message: (out): variable to store error message to (if any)
 *
 * Returns: whether the @argv was successfully executed (no error and exit code 0) or not
 */
gboolean bd_utils_exec_and_report_error (gchar **argv, gchar **error_message) {
    gboolean success = FALSE;
    GError *error = NULL;
    gint status = 0;
    gchar *stdout_data = NULL;
    gchar *stderr_data = NULL;

    success = g_spawn_sync (NULL, argv, NULL, G_SPAWN_DEFAULT|G_SPAWN_SEARCH_PATH,
                            NULL, NULL, &stdout_data, &stderr_data, &status, &error);

    if (!success) {
        *error_message = g_strdup (error->message);
        g_error_free(error);
        return FALSE;
    }

    if (status != 0) {
        if (stderr_data && (g_strcmp0 ("", stderr_data) != 0)) {
            *error_message = stderr_data;
            g_free (stdout_data);
        } else {
            *error_message = stdout_data;
            g_free (stderr_data);
        }

        return FALSE;
    }

    g_free (stdout_data);
    g_free (stderr_data);
    return TRUE;
}

/**
 * bd_utils_exec_and_capture_output:
 * @argv: (array zero-terminated=1): the argv array for the call
 * @output: (out): variable to store output to
 * @error_message: (out): variable to store error message to (if any)
 *
 * Returns: whether the @argv was successfully executed capturing the output or not
 */
gboolean bd_utils_exec_and_capture_output (gchar **argv, gchar **output, gchar **error_message) {
    gchar *stdout_data = NULL;
    gchar *stderr_data = NULL;
    GError *error = NULL;
    gint status = 0;
    gboolean success = FALSE;

    success = g_spawn_sync (NULL, argv, NULL, G_SPAWN_DEFAULT|G_SPAWN_SEARCH_PATH,
                            NULL, NULL, &stdout_data, &stderr_data, &status, &error);

    if (!success) {
        *error_message = g_strdup (error->message);
        g_error_free (error);
        return FALSE;
    }

    if ((status != 0) || (g_strcmp0 ("", stdout_data) == 0)) {
        if (stderr_data && (g_strcmp0 ("", stderr_data) != 0)) {
            *error_message = stderr_data;
            g_free (stdout_data);
        }

        return FALSE;
    } else {
        *output = stdout_data;
        g_free (stderr_data);
        return TRUE;
    }
}