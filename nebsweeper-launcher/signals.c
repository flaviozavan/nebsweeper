/*
 * NebSweeper Launcher - A server for NebSweeper
 * NebSweeper Launcher Copyright (C) 2011 Flávio Zavan
 *
 * This file is part of NebSweeper Launcher.
 *
 * NebSweeper Launcher is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NebSweeper Launcher is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NebSweeper Launcher.  If not, see <http://www.gnu.org/licenses/>.
 *
 * flavio [AT] nebososo [DOT] com
 * http://www.nebososo.com
*/

#include <gtk/gtk.h>
#include "signals.h"

void mainWindowDestroy(GtkWidget *w, gpointer d){
    gtk_main_quit();
}

void showData(GtkWidget *w, gpointer d){
    gtk_widget_show(d);
}

void hideItself(GtkWidget *w, gpointer d){
    gtk_widget_hide(w);
}
