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

#include <stdlib.h>
#include <gtk/gtk.h>
#include "signals.h"

#ifdef WIN32
#define G_MODULE_EXPORT __declspec(dllexport)
#endif

int main(int argc, char *argv[]){
    GtkBuilder *builder;
    GError *error = NULL;

    //Initialize GTK
    gtk_init(&argc, &argv);

    //Initialize the builder
    if(!(builder = gtk_builder_new())){
        fprintf(stderr, "Could not initialize GTK Builder.\n");
        exit(1);
    }
    
    //Read the style file
    if(!gtk_builder_add_from_file(builder, "data/launcher.glade", &error)){
        fprintf(stderr, "GTK Builder error code %d.\n%s.\n",
            error->code, error->message);

        exit(2);
    }

    //Connect the signals
    gtk_builder_connect_signals(builder, NULL);

    g_object_unref((gpointer) builder);

    gtk_main();

    return 0;
}
