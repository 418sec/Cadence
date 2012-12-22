/*
 * Carla UI bridge code
 * Copyright (C) 2011-2012 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the COPYING file
 */

#include "carla_bridge_client.hpp"
#include "carla_bridge_toolkit.hpp"

#if defined(BRIDGE_COCOA) || defined(BRIDGE_HWND) || defined(BRIDGE_X11)
# error Embed UI uses Qt
#endif

#include <gtk/gtk.h>
#include <QtCore/QSettings>

CARLA_BRIDGE_START_NAMESPACE

// -------------------------------------------------------------------------

#if defined(BRIDGE_GTK2)
static const char* const appName = "Carla-Gtk2UIs";
#elif defined(BRIDGE_GTK3)
static const char* const appName = "Carla-Gtk3UIs";
#else
static const char* const appName = "Carla-UIs";
#endif

static int    gargc = 0;
static char** gargv = {};

// -------------------------------------------------------------------------

class CarlaToolkitGtk : public CarlaBridgeToolkit
{
public:
    CarlaToolkitGtk(CarlaBridgeClient* const client, const char* const uiTitle)
        : CarlaBridgeToolkit(client, uiTitle),
          settings("Cadence", appName)
    {
        qDebug("CarlaToolkitGtk::CarlaToolkitGtk(%p, \"%s\")", client, uiTitle);

        window = nullptr;

        lastX = lastY = 0;
        lastWidth = lastHeight = 0;
    }

    ~CarlaToolkitGtk()
    {
        qDebug("CarlaToolkitGtk::~CarlaToolkitGtk()");
    }

    void init()
    {
        qDebug("CarlaToolkitGtk::init()");
        CARLA_ASSERT(! window);

        gtk_init(&gargc, &gargv);
    }

    void exec(const bool showGui)
    {
        qDebug("CarlaToolkitGtk::exec(%s)", bool2str(showGui));
        CARLA_ASSERT(client);

        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_container_add(GTK_CONTAINER(window), (GtkWidget*)client->getWidget());

        gtk_window_set_resizable(GTK_WINDOW(window), client->isResizable());
        gtk_window_set_title(GTK_WINDOW(window), uiTitle);

        gtk_window_get_position(GTK_WINDOW(window), &lastX, &lastY);
        gtk_window_get_size(GTK_WINDOW(window), &lastWidth, &lastHeight);

        if (settings.contains(QString("%1/pos_x").arg(uiTitle)))
        {
            lastX = settings.value(QString("%1/pos_x").arg(uiTitle), lastX).toInt();
            lastY = settings.value(QString("%1/pos_y").arg(uiTitle), lastY).toInt();
            gtk_window_move(GTK_WINDOW(window), lastX, lastY);

            if (client->isResizable())
            {
                lastWidth  = settings.value(QString("%1/width").arg(uiTitle), lastWidth).toInt();
                lastHeight = settings.value(QString("%1/height").arg(uiTitle), lastHeight).toInt();
                gtk_window_resize(GTK_WINDOW(window), lastWidth, lastHeight);
            }
        }

        g_timeout_add(50, gtk_ui_timeout, this);
        g_signal_connect(window, "destroy", G_CALLBACK(gtk_ui_destroy), this);

        if (showGui)
            show();
        else
            client->sendOscUpdate();

        // Main loop
        gtk_main();
    }

    void quit()
    {
        qDebug("CarlaToolkitGtk::quit()");

        if (window)
        {
            gtk_widget_destroy(window);
            gtk_main_quit();

            window = nullptr;
        }
    }

    void show()
    {
        qDebug("CarlaToolkitGtk::show()");
        CARLA_ASSERT(window);

        if (window)
            gtk_widget_show_all(window);
    }

    void hide()
    {
        qDebug("CarlaToolkitGtk::hide()");
        CARLA_ASSERT(window);

        if (window)
        {
#ifdef BRIDGE_GTK2
            gtk_widget_hide_all(window);
#else
            gtk_widget_hide(window);
#endif
        }
    }

    void resize(int width, int height)
    {
        qDebug("CarlaToolkitGtk::resize(%i, %i)", width, height);
        CARLA_ASSERT(window);

        if (window)
            gtk_window_resize(GTK_WINDOW(window), width, height);
    }

    // ---------------------------------------------------------------------

protected:
    void handleDestroy()
    {
        qDebug("CarlaToolkitGtk::handleDestroy()");

        window = nullptr;

        settings.setValue(QString("%1/pos_x").arg(uiTitle), lastX);
        settings.setValue(QString("%1/pos_y").arg(uiTitle), lastY);
        settings.setValue(QString("%1/width").arg(uiTitle), lastWidth);
        settings.setValue(QString("%1/height").arg(uiTitle), lastHeight);
        settings.sync();
    }

    gboolean handleTimeout()
    {
        if (window)
        {
            gtk_window_get_position(GTK_WINDOW(window), &lastX, &lastY);
            gtk_window_get_size(GTK_WINDOW(window), &lastWidth, &lastHeight);
        }

        // FIXME
        return client->isOscControlRegistered() ? client->oscIdle() : false;
    }

    // ---------------------------------------------------------------------

private:
    GtkWidget* window;
    QSettings settings;

    gint lastX, lastY, lastWidth, lastHeight;

    static void gtk_ui_destroy(GtkWidget*, gpointer data)
    {
        CarlaToolkitGtk* const _this_ = (CarlaToolkitGtk*)data;
        _this_->handleDestroy();

        gtk_main_quit();
    }

    static gboolean gtk_ui_timeout(gpointer data)
    {
        CarlaToolkitGtk* const _this_ = (CarlaToolkitGtk*)data;
        return _this_->handleTimeout();
    }
};

// -------------------------------------------------------------------------

CarlaBridgeToolkit* CarlaBridgeToolkit::createNew(CarlaBridgeClient* const client, const char* const uiTitle)
{
    return new CarlaToolkitGtk(client, uiTitle);
}

CARLA_BRIDGE_END_NAMESPACE
