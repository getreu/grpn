/*

  Copyright (C) 2002  Paul Wilkins

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
/* process_input.c  by Paul Wilkins 3/21/97 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "process_input.h"
#include "funcs.h"
#include "lcd.h"
#include "editor.h"
#include "error.h"
#include "number.h"
#include "stack.h"
#include <locale.h>

void processInput(int ksym, int isCtrl, char bb, char chr){
    int keysym = ksym;
    char c;

    if (isCtrl) {
	GtkClipboard* clipboard;
	Number *num;
	gchar* cliptext;

	switch(keysym) {
	case GDK_c:      /*copy to clipboard*/
	    num = getStackEle(0);
	    if (num == NULL)
		break;
	    cliptext = printNumber(num);
	    if (cliptext == NULL)
		break;

	    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	    gtk_clipboard_set_text(clipboard, cliptext, -1);
	    gtk_clipboard_store(clipboard);
	    free(cliptext);
	    break;
	}
	/* ignore the ctrl key */
	return;
    }

    /* this will clear any error string */
    resetError();

    /* process the line */
    switch(keysym) {
    case GDK_space:     /* if there is something in the buffer, then */
    case GDK_KP_Space:  /* push it onto the stack, else duplicate the */
    case GDK_Return:    /* element on the bottom of the stack */
    case GDK_Tab:
    case GDK_KP_Tab:
    case GDK_KP_Enter:
    case GDK_KP_Insert:
    case GDK_Linefeed:
	PushStack();
	break;

    case GDK_Begin:      /* used when editing the entry buffer */
    case GDK_Home:
    case GDK_Up:
    case GDK_KP_Home:
    case GDK_KP_Up:
	homeEditor();
	break;

    case GDK_End:      /* used when editing the entry buffer */
    case GDK_Down:
    case GDK_KP_End:
    case GDK_KP_Down:
	endEditor();
	break;

    case GDK_Right:      /* used when editing the entry buffer */
    case GDK_KP_Right:
	rightEditor();
	break;

    case GDK_Left:      /* used when editing the entry buffer */
    case GDK_KP_Left:
	leftEditor();
	break;

    case GDK_BackSpace:  /* delete the last entry in the stack */
    case GDK_Delete:     /* OR backspace over the last char typed */
    case GDK_KP_Delete:
	if (isEditingEditor()) {
            deleteEditor();
	} else {
            PopStack();
	}
	break;

    case GDK_Escape:  /* clear the entry buffer */
	cancelEditor();
	break;

    case GDK_plus: 
    case GDK_KP_Add:
	c = getPrevEditor();
	if (c == 'e' || c == 'E'){
            insertEditor(bb);
	} else {
	    AddStack();
	}
	break;

    case GDK_minus: 
    case GDK_KP_Subtract:
	c = getPrevEditor();
	if (c == 'e' || c == 'E'){
            insertEditor(bb);
	} else {
	    SubStack();
	}
	break;

    case GDK_asterisk: 
    case GDK_KP_Multiply:
	MulStack();
	break;

    case GDK_slash: 
    case GDK_KP_Divide:
	DivStack();
	break;

    case GDK_asciicircum: // '^'
	PowStack();
	break;

    default:  /* catch everything that's left over */

	/* ascii characters */
         
	if (bb == '.') {
	    //is the locale decimal seperator a comma?
	    struct lconv * lc;
	    lc = localeconv();
	    if (strcmp(lc->decimal_point, ",") == 0) {
		// then replace . by ,
		bb = ',';
	    }
	} else if (bb == ',') {
	    //is the locale decimal seperator a comma?
	    struct lconv * lc;
	    lc = localeconv();
	    if  (strcmp(lc->decimal_point, ".") == 0) {
		// then replace , by .
		bb='.';
	    }
	}

	if ((keysym >= GDK_KP_Space && keysym <= GDK_KP_9) ||
	    (keysym >= GDK_space && keysym <= GDK_asciitilde)){
            insertEditor(bb);
	}

	break;
    }

    /* repaint the screen */
    redrawLCD();
}
