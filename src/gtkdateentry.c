/*
 * GtkDateEntry widget for GTK+
 *
 * Copyright (C) 2005-2010 Andrea Zagli <azagli@libero.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <string.h>

#include <gdk/gdkkeysyms.h>

#include <gtk/gtkmain.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtkcalendar.h>

#include <gtkmaskedentry.h>

#include "gtkdateentry.h"

enum
{
	PROP_0,
	PROP_EDITABLE_WITH_CALENDAR
};

static void gtk_date_entry_class_init (GtkDateEntryClass *klass);
static void gtk_date_entry_init (GtkDateEntry *date);

static void gtk_date_entry_change_mask (GtkDateEntry *date);

static void hide_popup (GtkWidget *date);
static gboolean popup_grab_on_window (GdkWindow *window,
                                      guint32 activate_time);
static gint delete_popup (GtkWidget *widget,
                          gpointer data);
static gint key_press_popup (GtkWidget *widget,
                             GdkEventKey *event,
                             gpointer data);
static gint button_press_popup (GtkWidget *widget,
                                GdkEventButton *event,
                                gpointer user_data);
static void btnCalendar_on_toggled (GtkToggleButton *togglebutton,
                                    gpointer user_data);
static void calendar_on_day_selected (GtkCalendar *calendar,
                                      gpointer user_data);
static void calendar_on_day_selected_double_click (GtkCalendar *calendar,
                                                   gpointer user_data);

static void gtk_date_entry_set_property (GObject *object,
                                           guint property_id,
                                           const GValue *value,
                                           GParamSpec *pspec);
static void gtk_date_entry_get_property (GObject *object,
                                           guint property_id,
                                           GValue *value,
                                           GParamSpec *pspec);

static GtkWidgetClass *parent_class = NULL;


#define GTK_DATE_ENTRY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_DATE_ENTRY, GtkDateEntryPrivate))

typedef struct _GtkDateEntryPrivate GtkDateEntryPrivate;
struct _GtkDateEntryPrivate
	{
		GtkWidget *day;
		GtkWidget *btnCalendar;
		GtkWidget *wCalendar;
		GtkWidget *calendar;

		gchar separator;
		gchar *format;
		gboolean editable_with_calendar;
	};

GType
gtk_date_entry_get_type (void)
{
	static GType date_type = 0;

	if (!date_type)
		{
			static const GTypeInfo date_info =
			{
				sizeof (GtkDateEntryClass),
				NULL,		/* base_init */
				NULL,		/* base_finalize */
				(GClassInitFunc) gtk_date_entry_class_init,
				NULL,		/* class_finalize */
				NULL,		/* class_data */
				sizeof (GtkDateEntry),
				0,		/* n_preallocs */
				(GInstanceInitFunc) gtk_date_entry_init,
			};

			date_type = g_type_register_static (GTK_TYPE_HBOX, "GtkDateEntry",
			                                    &date_info, 0);
		}

	return date_type;
}

static void
gtk_date_entry_class_init (GtkDateEntryClass *klass)
{
	GtkWidgetClass *widget_class;

	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (GtkDateEntryPrivate));

	widget_class = (GtkWidgetClass*) klass;
	parent_class = g_type_class_peek_parent (klass);

	object_class->set_property = gtk_date_entry_set_property;
	object_class->get_property = gtk_date_entry_get_property;

	g_object_class_install_property (object_class, PROP_EDITABLE_WITH_CALENDAR,
	                                 g_param_spec_boolean ("editable-with-calendar",
	                                                       "TRUE if it is editable only from calendar",
	                                                       "Determines if the user can edit the text"
	                                                       " in the #GtkDateEntry widget only from the calendar or not",
	                                                       FALSE,
	                                                       G_PARAM_READWRITE));
}

static void
gtk_date_entry_init (GtkDateEntry *date)
{
	GtkWidget *arrow;

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	/* TO DO
	 * read separator and format from locale settings
	 */
	priv->separator = '/';
	priv->format = g_strdup ("dmY");

	priv->day = gtk_masked_entry_new ();
	gtk_date_entry_change_mask (date);
	gtk_box_pack_start (GTK_BOX (date), priv->day, TRUE, TRUE, 0);
	gtk_widget_show (priv->day);

	priv->btnCalendar = gtk_toggle_button_new ();
	gtk_box_pack_start (GTK_BOX (date), priv->btnCalendar, FALSE, FALSE, 0);
	gtk_widget_set_no_show_all (priv->btnCalendar, TRUE);
	gtk_widget_show (priv->btnCalendar);

	g_signal_connect (G_OBJECT (priv->btnCalendar), "toggled",
	                  G_CALLBACK (btnCalendar_on_toggled), (gpointer)date);

	arrow = (GtkWidget *)gtk_arrow_new (GTK_ARROW_DOWN, GTK_SHADOW_NONE);
	gtk_container_add (GTK_CONTAINER (priv->btnCalendar), arrow);
	gtk_widget_show (arrow);

	priv->wCalendar = gtk_window_new (GTK_WINDOW_POPUP);
	gtk_window_set_resizable (GTK_WINDOW (priv->wCalendar), FALSE);
	gtk_container_set_border_width (GTK_CONTAINER (priv->wCalendar), 3);

	gtk_widget_set_events (priv->wCalendar,
	                       gtk_widget_get_events (priv->wCalendar) | GDK_KEY_PRESS_MASK);

	g_signal_connect (priv->wCalendar, "delete_event",
	                  G_CALLBACK (delete_popup), date);
	g_signal_connect (priv->wCalendar, "key_press_event",
	                  G_CALLBACK (key_press_popup), date);
	g_signal_connect (priv->wCalendar, "button_press_event",
	                  G_CALLBACK (button_press_popup), date);

	priv->calendar = gtk_calendar_new ();
	gtk_container_add (GTK_CONTAINER (priv->wCalendar), priv->calendar);
	gtk_widget_show (priv->calendar);

	g_signal_connect (G_OBJECT (priv->calendar), "day-selected",
										G_CALLBACK (calendar_on_day_selected), (gpointer)date);
	g_signal_connect (G_OBJECT (priv->calendar), "day-selected-double-click",
										G_CALLBACK (calendar_on_day_selected_double_click), (gpointer)date);
}

/**
 * gtk_date_entry_new:
 * @format: a #gchar which is the date's format.
 * Possible values are:
 * - d: day of the month with leading zeroes
 * - m: month of the year with leading zeroes
 * - Y: year with century.
 * @separator: a #gchar that represents the separator between day, month and year.
 * @calendar_button_is_visible: if calendar button is visible.
 *
 * Creates a new #GtkDateEntry.
 *
 * Returns: The newly created #GtkDateEntry widget.
 */
GtkWidget
*gtk_date_entry_new (const gchar *format, const gchar separator, gboolean calendar_button_is_visible)
{
	GtkWidget *w = GTK_WIDGET (g_object_new (gtk_date_entry_get_type (), NULL));

	if (!gtk_date_entry_set_format (GTK_DATE_ENTRY (w), format)) return NULL;
	gtk_date_entry_set_separator (GTK_DATE_ENTRY (w), separator);

	gtk_date_entry_set_calendar_button_visible (GTK_DATE_ENTRY (w), calendar_button_is_visible);

	return w;
}

/**
 * gtk_date_entry_set_separator:
 * @date: a #GtkDateEntry object.
 * @separator: a #gchar that represents the separator between day, month and year.
 *
 * Set the separator between day, month and year.
 */
void
gtk_date_entry_set_separator (GtkDateEntry *date, const gchar separator)
{
	GDate *gdate = gtk_date_entry_get_gdate (date);

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	priv->separator = separator;
	gtk_date_entry_change_mask (date);
	gtk_date_entry_set_date_gdate (date, gdate);
}

/**
 * gtk_date_entry_set_format:
 * @date: a #GtkDateEntry object.
 * @format: a #gchar which is the date's format.
 *
 * Sets the date's format.
 *
 * Returns: FALSE if @format isn't a valid date format.
 */
gboolean
gtk_date_entry_set_format (GtkDateEntry *date, const gchar *format)
{
	gint i;
	gboolean d = FALSE;
	gboolean m = FALSE;
	gboolean y = FALSE;
	gchar *format_;
	GDate *gdate;

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	if (format == NULL)
		{
			return FALSE;
		}
	
	format_ = g_strstrip (g_strdup (format));
	if (strlen (format_) != 3)
		{
			return FALSE;
		}

	for (i = 0; i < 3; i++)
		{
			switch (format[i])
				{
					case 'd':
						if (d) return FALSE;
						d = TRUE;
						break;

					case 'm':
						if (m) return FALSE;
						m = TRUE;
						break;

					case 'Y':
						if (y) return FALSE;
						y = TRUE;
						break;

					default:
						return FALSE;
				}
		}
	if (!d || !m || !y) return FALSE; 

	gdate = gtk_date_entry_get_gdate (date);

	priv->format = g_strdup (format);
	gtk_date_entry_change_mask (date);
	gtk_date_entry_set_date_gdate (date, gdate);

	return TRUE;
}

/**
 * gtk_date_entry_get_text:
 * @date: a #GtkDateEntry object.
 *
 * Returns the @date's content as is.
 *
 * Returns: A pointer to the content of the widget as is.
 */
const gchar
*gtk_date_entry_get_text (GtkDateEntry *date)
{
	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	return gtk_entry_get_text (GTK_ENTRY (priv->day));
}

/**
 * gtk_date_entry_get_strf:
 * @date: a #GtkDateEntry object.
 * @format: a #gchar which is the date's format.
 * @separator: a #gchar which is the separator between day, month and year.
 *
 * Returns: A pointer to the content of the widget formatted as specified in 
 * @format with @separator.
 */
const gchar
*gtk_date_entry_get_strf (GtkDateEntry *date,
                          const gchar *format,
                          gchar separator)
{
	gchar *fmt, sep, *ret = "";
	gint i;
	GDate *gdate = (GDate *)gtk_date_entry_get_gdate (date);

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	if (gdate == NULL)
		{
			return "";
		}

	if (format == NULL)
		{
			fmt = priv->format;
		}
	else
		{
			fmt = g_strdup (format);
		}
	if (separator == 0)
		{
			sep = priv->separator;
		}
	else
		{
			sep = (gchar)separator;
		}

	for (i = 0; i < 3; i++)
		{
			switch (fmt[i])
				{
					case 'd':
						ret = g_strjoin (NULL, ret, g_strdup_printf ("%02d", (int)g_date_get_day (gdate)), NULL);
						break;

					case 'm':
						ret = g_strjoin (NULL, ret, g_strdup_printf ("%02d", (int)g_date_get_month (gdate)), NULL);
						break;

					case 'Y':
						ret = g_strjoin (NULL, ret, g_strdup_printf ("%04d", (int)g_date_get_year (gdate)), NULL);
						break;
				}

			if (i < 2)
				{
					ret = g_strjoin (NULL, ret, g_strdup_printf ("%c", sep), NULL);
				}
		}

	return (const gchar *)ret;
}

/**
 * gtk_date_entry_get_tm:
 * @date: a #GtkDateEntry object.
 *
 * Returns: the @date's content as a struct tm.
 */
struct tm
*gtk_date_entry_get_tm (GtkDateEntry *date)
{
	struct tm tm;

	const GDate *gdate = gtk_date_entry_get_gdate (date);

	if (gdate == NULL) return NULL;

	g_date_to_struct_tm (gdate, &tm);

	return (struct tm *)g_memdup ((gconstpointer)&tm, sizeof (struct tm));
}

/**
 * gtk_date_entry_get_gdate:
 * @date: a #GtkDateEntry object.
 *
 * Returns: the @date's content as a #GDate.
 */
GDate
*gtk_date_entry_get_gdate (GtkDateEntry *date)
{
	gint i, pos = 0, val;
	gchar *txt;
	GDate *gdate;

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	txt = (gchar *)gtk_entry_get_text (GTK_ENTRY (priv->day));
	gdate = g_date_new ();

	for (i = 0; i < 3; i++)
		{
			switch (priv->format[i])
				{
					case 'd':
						val = atol (g_strndup (txt + pos, 2));
						if (g_date_valid_day ((GDateDay)val))
							{
								g_date_set_day (gdate, (GDateDay)val);
								pos += 3;
							}
						else
							{
								return NULL;
							}
						break;

					case 'm':
						val = atol (g_strndup (txt + pos, 2));
						if (g_date_valid_month ((GDateMonth)val))
							{
								g_date_set_month (gdate, (GDateMonth)val);
								pos += 3;
							}
						else
							{
								return NULL;
							}
						break;

					case 'Y':
						val = atol (g_strndup (txt + pos, 4));
						if (g_date_valid_year ((GDateYear)val))
							{
								g_date_set_year (gdate, (GDateYear)val);
								pos += 5;
							}
						else
							{
								return NULL;
							}
						break;
				}
		}

	return gdate;
}

/**
 * gtk_date_entry_set_date_strf:
 * @date: a #GtkDateEntry.
 * @str: a #gchar which is the content to set.
 * @format: a #gchar which is the date's format.
 * @separator: a #gchar which is the separator between day, month and year.
 *
 * Sets @date's content from the @str string and based on @format and @separator, 
 * if it's a valid date.
 *
 * Returns: TRUE if @date's content is setted.
 */
gboolean
gtk_date_entry_set_date_strf (GtkDateEntry *date,
                              const gchar *str,
                              const gchar *format,
                              const gchar separator)
{
	gchar *fmt;
	gchar sep;
	GDateDay day;
	GDateMonth month;
	GDateYear year;
	gint i;
	gint pos = 0;

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	if (format == NULL)
		{
			fmt = priv->format;
		}
	else
		{
			fmt = g_strdup (format);
		}
	if (separator == 0)
		{
			sep = priv->separator;
		}
	else
		{
			sep = (gchar)separator;
		}

	for (i = 0; i < 3; i++)
		{
			switch (fmt[i])
				{
					case 'd':
						if (!g_date_valid_day ((GDateDay)atol (g_strndup (str + pos, 2)))) return FALSE;
						/*txt = g_strjoin (NULL, txt, g_strndup (str + pos, 2), NULL);*/
						day = (GDateDay)atol (g_strndup (str + pos, 2));
						pos += 3;
						break;

					case 'm':
						if (!g_date_valid_month ((GDateMonth)atol (g_strndup (str + pos, 2)))) return FALSE;
						/*txt = g_strjoin (NULL, txt, g_strndup (str + pos, 2), NULL);*/
						month = (GDateMonth)atol (g_strndup (str + pos, 2));
						pos += 3;
						break;

					case 'Y':
						if (!g_date_valid_year ((GDateYear)atol (g_strndup (str + pos, 4)))) return FALSE;
						/*txt = g_strjoin (NULL, txt, g_strndup (str + pos, 4), NULL);*/
						year = (GDateYear)atol (g_strndup (str + pos, 4));
						pos += 5;
						break;
				}

			/*if (i < 2)
				{
					txt = g_strjoin (NULL, txt, g_strdup_printf ("%c", sep), NULL);
				}*/
		}

	/*gtk_entry_set_text (GTK_ENTRY (date->day), txt);*/
	gtk_date_entry_set_date_gdate (date, g_date_new_dmy (day, month, year));
	return TRUE;
}

/**
 * gtk_date_entry_set_date_tm:
 * @date: a #GtkDateEntry.
 * @tmdate: a tm struct from which set @date's content.
 *
 * Sets @date's content from the tm struct @tmdate.
 **/
void
gtk_date_entry_set_date_tm (GtkDateEntry *date, const struct tm tmdate)
{
	GDate *gdate = g_date_new_dmy ((GDateDay)tmdate.tm_mday,
	                               (GDateMonth)tmdate.tm_mon + 1,
	                               (GDateYear)tmdate.tm_year + 1900);
	gtk_date_entry_set_date_gdate (date, (const GDate *)gdate);
}

/**
 * gtk_date_entry_set_date_gdate:
 * @date: a #GtkDateEntry.
 * @gdate: a #GDate from which set @date's content.
 *
 * Sets @date's content from a @gdate.
 **/
void
gtk_date_entry_set_date_gdate (GtkDateEntry *date, const GDate *gdate)
{
	gint i;
	gchar *txt = "";

	if (gdate == NULL || !g_date_valid (gdate)) return;

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	for (i = 0; i < 3; i++)
		{
			switch (priv->format[i])
				{
					case 'd':
						txt = g_strjoin (NULL, txt, g_strdup_printf ("%02d", g_date_get_day (gdate)), NULL);
						break;

					case 'm':
						txt = g_strjoin (NULL, txt, g_strdup_printf ("%02d", g_date_get_month (gdate)), NULL);
						break;

					case 'Y':
						txt = g_strjoin (NULL, txt, g_strdup_printf ("%04d", g_date_get_year (gdate)), NULL);
						break;
				}

			if (i < 2)
				{
					txt = g_strjoin (NULL, txt, g_strdup_printf ("%c", priv->separator), NULL);
				}
		}

	gtk_entry_set_text (GTK_ENTRY (priv->day), txt);
}

/**
 * gtk_date_entry_is_valid:
 * @date: a #GtkDateEntry.
 *
 * Checks if @date's content is a valid date.
 *
 * Returns: TRUE if @date's content is a valid date.
 **/
gboolean
gtk_date_entry_is_valid (GtkDateEntry *date)
{
	const GDate *gdate = gtk_date_entry_get_gdate (date);
	if (gdate != NULL)
		{
			return g_date_valid (gdate);
		}
	else
		{
			return FALSE;
		}
}

/**
 * gtk_date_entry_set_editable:
 * @date: a #GtkDateEntry.
 * @is_editable: TRUE if the user is allowed to edit the text in the widget.
 *
 * Determines if the user can edit the text in the #GtkDateEntry widget or not.
 */
void
gtk_date_entry_set_editable (GtkDateEntry *date,
                             gboolean is_editable)
{
	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	gtk_editable_set_editable (GTK_EDITABLE (priv->day), is_editable);
	gtk_widget_set_sensitive (priv->btnCalendar, is_editable);
}

/**
 * gtk_date_entry_set_editable_with_calendar:
 * @date:  a #GtkDateEntry.
 * @is_editable_with_calendar: TRUE if the user is allowed to edit the text
 * in the widget only from the calendar.
 *
 * Determines if the user can edit the text in the #GtkDateEntry widget only
 * from the calendar or not.
 */
void
gtk_date_entry_set_editable_with_calendar (GtkDateEntry *date,
                                  gboolean is_editable_with_calendar)
{
	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	gtk_editable_set_editable (GTK_EDITABLE (priv->day), !is_editable_with_calendar);
}

/**
 * gtk_date_entry_set_calendar_button_visible:
 * @date: a #GtkDateEntry.
 * @is_visible: TRUE if the calendar's button must be visible.
 * 
 * Determines if the calendar's button is visible or not.
 */
void
gtk_date_entry_set_calendar_button_visible (GtkDateEntry *date,
                                            gboolean is_visible)
{
	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	if (is_visible)
		{
			gtk_widget_show (priv->btnCalendar);
		}
	else
		{
			gtk_widget_hide (priv->btnCalendar);
		}
}

/* PRIVATE */
static void
gtk_date_entry_change_mask (GtkDateEntry *date)
{
	gchar *mask, *format[3];
	gint i;

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);

	for (i = 0; i < 3; i++)
		{
			if (priv->format[i] == 'd' || priv->format[i] == 'm')
				{
					format[i] = g_strdup ("00");
				}
			else if (priv->format[i] == 'Y')
				{
					format[i] = g_strdup ("0000");
				}
		}

	mask = g_strdup_printf ("%s%c%s%c%s",
	                        format[0],
	                        priv->separator,
	                        format[1],
	                        priv->separator,
	                        format[2]);
	gtk_masked_entry_set_mask (GTK_MASKED_ENTRY (priv->day), mask);
}

/*
 * callbacks
 **/
static void
hide_popup (GtkWidget *date)
{
	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE ((GtkDateEntry *)date);

	gtk_widget_hide (priv->wCalendar);
	gtk_grab_remove (priv->wCalendar);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (priv->btnCalendar), FALSE);
}

static gboolean
popup_grab_on_window (GdkWindow *window,
                      guint32 activate_time)
{
	if ((gdk_pointer_grab (window, TRUE,
	                       GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
	                       GDK_POINTER_MOTION_MASK,
	                       NULL, NULL, activate_time) == 0))
		{
			if (gdk_keyboard_grab (window, TRUE, activate_time) == 0)
				{
					return TRUE;
				}
			else
				{
					gdk_pointer_ungrab (activate_time);
					return FALSE;
				}
		}

	return FALSE;
}

static gint
delete_popup (GtkWidget *widget,
              gpointer data)
{
	hide_popup ((GtkWidget *)data);

	return TRUE;
}

static gint
key_press_popup (GtkWidget *widget,
                 GdkEventKey *event,
                 gpointer data)
{
	switch (event->keyval)
		{
			case GDK_Escape:
				break;

			case GDK_Return:
	    case GDK_KP_Enter:
				/* TO DO */
				break;

			default:
				return FALSE;
		}

	g_signal_stop_emission_by_name (widget, "key_press_event");
	hide_popup ((GtkWidget *)data);

	return TRUE;
}

/* This function is yanked from gtkcombo.c */
static gint
button_press_popup (GtkWidget *widget,
                    GdkEventButton *event,
                    gpointer user_data)
{
	GtkDateEntry *date = (GtkDateEntry *)user_data;
	GtkWidget *child = gtk_get_event_widget ((GdkEvent *)event);

	/* We don't ask for button press events on the grab widget, so
	 *  if an event is reported directly to the grab widget, it must
	 *  be on a window outside the application (and thus we remove
	 *  the popup window). Otherwise, we check if the widget is a child
	 *  of the grab widget, and only remove the popup window if it
	 *  is not.
	 */
	if (child != widget) {
		while (child) {
			if (child == widget) return FALSE;
			child = child->parent;
		}
	}

	hide_popup (user_data);

	return TRUE;
}

static void
btnCalendar_on_toggled (GtkToggleButton *togglebutton,
                        gpointer user_data)
{
	if (gtk_toggle_button_get_active (togglebutton))
		{
			GtkDateEntry *date = (GtkDateEntry *)user_data;
			GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date);
		
			gint x, y, bwidth, bheight;
			GtkRequisition req;
			GtkWidget *btn = priv->btnCalendar,
			          *wCalendar = priv->wCalendar;

			/* sets current date */
			const GDate *gdate = gtk_date_entry_get_gdate (date);
			if (gdate != NULL)
				{
					gtk_calendar_select_month (GTK_CALENDAR (priv->calendar),
					                           (guint)g_date_get_month (gdate) - 1,
					                           (guint)g_date_get_year (gdate));
					gtk_calendar_select_day (GTK_CALENDAR (priv->calendar),
					                         (guint)g_date_get_day (gdate));
				}

			/* show calendar */
			gtk_widget_size_request (wCalendar, &req);
			gdk_window_get_origin (GDK_WINDOW (btn->window), &x, &y);
			x += btn->allocation.x;
			y += btn->allocation.y;
			bwidth = btn->allocation.width;
			bheight = btn->allocation.height;
			x += bwidth - req.width;
			y += bheight;
			if (x < 0) x = 0;
			if (y < 0) y = 0;

			gtk_editable_set_position (GTK_EDITABLE (priv->day), 0);

			gtk_grab_add (wCalendar);
			gtk_window_move (GTK_WINDOW (wCalendar), x, y);
			gtk_widget_show (wCalendar);
			gtk_widget_grab_focus (priv->calendar);
			popup_grab_on_window (wCalendar->window, gtk_get_current_event_time ());
		}
}

static void
calendar_on_day_selected (GtkCalendar *calendar,
                          gpointer user_data)
{
	guint day, month, year;

	gtk_calendar_get_date (calendar, &year, &month, &day);
	gtk_date_entry_set_date_gdate ((GtkDateEntry *)user_data,
                           g_date_new_dmy ((GDateDay)day,
                                           (GDateMonth)(month + 1),
                                           (GDateYear)year));
}

static void
calendar_on_day_selected_double_click (GtkCalendar *calendar,
                                       gpointer user_data)
{
	hide_popup ((GtkWidget *)user_data);
}

static void
gtk_date_entry_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GtkDateEntry *date_entry = GTK_DATE_ENTRY (object);

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date_entry);

	switch (property_id)
		{
			case PROP_EDITABLE_WITH_CALENDAR:
				gtk_date_entry_set_editable_with_calendar (date_entry, g_value_get_boolean (value));
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
gtk_date_entry_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GtkDateEntry *date_entry = GTK_DATE_ENTRY (object);

	GtkDateEntryPrivate *priv = GTK_DATE_ENTRY_GET_PRIVATE (date_entry);

	switch (property_id)
		{
			case PROP_EDITABLE_WITH_CALENDAR:
				g_value_set_boolean (value, priv->editable_with_calendar);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
