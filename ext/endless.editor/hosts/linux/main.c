/*
 * Copyright (c) 2026 dev4fun. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms, as
 * designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 * This program is distributed in the hope that it will be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <gtk/gtk.h>
#include <webkit/webkit.h>

static void activate(GtkApplication *application, gpointer user_data) {
    (void)user_data;

    gchar *web_root = g_strdup(g_getenv("ENDLESS_CONTROL_WEB_ROOT"));
    if (web_root == NULL || *web_root == '\0') {
        gchar *current_dir = g_get_current_dir();
        g_free(web_root);
        web_root = g_build_filename(current_dir, "web", "dist", NULL);
        g_free(current_dir);
    }

    gchar *page_path = g_build_filename(web_root, "index.html", NULL);
    const gchar *development_url = g_getenv("ENDLESS_CONTROL_WEB_URL");
    gboolean use_development_url = development_url != NULL && *development_url != '\0';
    if (use_development_url && !g_str_has_prefix(development_url, "http://") && !g_str_has_prefix(development_url, "https://")) {
        g_printerr("ENDLESS_CONTROL_WEB_URL must be an HTTP or HTTPS URL.\n");
        g_free(page_path);
        g_free(web_root);
        g_application_quit(G_APPLICATION(application));
        return;
    }
    if (!use_development_url && !g_file_test(page_path, G_FILE_TEST_IS_REGULAR)) {
        g_printerr("React app not found at %s. Build it with `npm --prefix web run build`.\n", page_path);
        g_free(page_path);
        g_free(web_root);
        g_application_quit(G_APPLICATION(application));
        return;
    }

    GtkWidget *window = gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(window), "Endless Control");
    gtk_window_set_default_size(GTK_WINDOW(window), 1120, 760);

    WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gchar *page_uri = use_development_url ? g_strdup(development_url) : g_filename_to_uri(page_path, NULL, NULL);
    webkit_web_view_load_uri(web_view, page_uri);
    gtk_window_set_child(GTK_WINDOW(window), GTK_WIDGET(web_view));
    gtk_window_present(GTK_WINDOW(window));

    g_free(page_uri);
    g_free(page_path);
    g_free(web_root);
}

int main(int argc, char **argv) {
    GtkApplication *application = gtk_application_new(
        "com.endlesscontrol.desktop",
        G_APPLICATION_DEFAULT_FLAGS
    );
    g_signal_connect(application, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return status;
}
