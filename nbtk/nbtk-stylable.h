/* nbtk-stylable.h: Interface for stylable objects
 *
 * Copyright (C) 2008 Intel Corporation
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
 *
 * Written by: Emmanuele Bassi <ebassi@openedhand.com>
 *             Thomas Wood <thomas@linux.intel.com>
 */

#ifndef __NBTK_STYLABLE_H__
#define __NBTK_STYLABLE_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define NBTK_TYPE_STYLABLE              (nbtk_stylable_get_type ())
#define NBTK_STYLABLE(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), NBTK_TYPE_STYLABLE, NbtkStylable))
#define NBTK_IS_STYLABLE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NBTK_TYPE_STYLABLE))
#define NBTK_STYLABLE_IFACE(iface)      (G_TYPE_CHECK_CLASS_CAST ((iface), NBTK_TYPE_STYLABLE, NbtkStylableIface))
#define NBTK_IS_STYLABLE_IFACE(iface)   (G_TYPE_CHECK_CLASS_TYPE ((iface), NBTK_TYPE_STYLABLE))
#define NBTK_STYLABLE_GET_IFACE(obj)    (G_TYPE_INSTANCE_GET_INTERFACE ((obj), NBTK_TYPE_STYLABLE, NbtkStylableIface))

typedef struct _NbtkStylable            NbtkStylable; /* dummy typedef */
typedef struct _NbtkStylableIface       NbtkStylableIface;

#include <nbtk-style.h>

struct _NbtkStylableIface
{
  GTypeInterface g_iface;

  /* virtual functions */
  NbtkStyle *(* get_style) (NbtkStylable *stylable);
  void       (* set_style) (NbtkStylable *stylable,
                            NbtkStyle    *style);

  /* context virtual functions */
  NbtkStylable *(*get_container)    (NbtkStylable *stylable);
  NbtkStylable *(*get_base_style)   (NbtkStylable *stylable);
  const gchar  *(*get_style_id)     (NbtkStylable *stylable);
  const gchar  *(*get_style_type)   (NbtkStylable *stylable);
  const gchar  *(*get_style_class)  (NbtkStylable *stylable);
  const gchar  *(*get_pseudo_class) (NbtkStylable *stylable);
  gchar        *(*get_attribute)    (NbtkStylable *stylable, const gchar *name);
  gboolean      (*get_viewport)     (NbtkStylable *stylable,
                                    gint *x, gint *y,
                                    gint *width, gint *height);

  /* signals, not vfuncs */
  void (* style_notify) (NbtkStylable *stylable,
                         GParamSpec   *pspec);

  void (* style_set)    (NbtkStylable *stylable,
                         NbtkStyle    *old_style);
};

GType        nbtk_stylable_get_type               (void) G_GNUC_CONST;

void         nbtk_stylable_iface_install_property (NbtkStylableIface *iface,
                                                   GType              owner_type,
                                                   GParamSpec        *pspec);

void         nbtk_stylable_freeze_notify          (NbtkStylable      *stylable);
void         nbtk_stylable_notify                 (NbtkStylable      *stylable,
                                                   const gchar       *property_name);
void         nbtk_stylable_thaw_notify            (NbtkStylable      *stylable);
GParamSpec **nbtk_stylable_list_properties        (NbtkStylable      *stylable,
                                                   guint             *n_props);
GParamSpec * nbtk_stylable_find_property          (NbtkStylable      *stylable,
                                                   const gchar       *property_name);
void         nbtk_stylable_set_style              (NbtkStylable      *stylable,
                                                   NbtkStyle         *style);
NbtkStyle *  nbtk_stylable_get_style              (NbtkStylable      *stylable);

void         nbtk_stylable_set                    (NbtkStylable      *stylable,
                                                   const gchar       *first_property_name,
                                                   ...) G_GNUC_NULL_TERMINATED;
void         nbtk_stylable_get                    (NbtkStylable      *stylable,
                                                   const gchar       *first_property_name,
                                                   ...) G_GNUC_NULL_TERMINATED;
void         nbtk_stylable_set_property           (NbtkStylable      *stylable,
                                                   const gchar       *property_name,
                                                   const GValue      *value);
void         nbtk_stylable_get_property           (NbtkStylable      *stylable,
                                                   const gchar       *property_name,
                                                   GValue            *value);
NbtkStylable* nbtk_stylable_get_container     (NbtkStylable *stylable);
NbtkStylable* nbtk_stylable_get_base_style    (NbtkStylable *stylable);
const gchar*  nbtk_stylable_get_style_id      (NbtkStylable *stylable);
const gchar*  nbtk_stylable_get_style_type    (NbtkStylable *stylable);
const gchar*  nbtk_stylable_get_style_class   (NbtkStylable *stylable);
const gchar*  nbtk_stylable_get_pseudo_class  (NbtkStylable *stylable);
gchar*        nbtk_stylable_get_attribute     (NbtkStylable *stylable, const gchar *name);
gboolean      nbtk_stylable_get_viewport      (NbtkStylable *stylable,
                                               gint *x, gint *y,
                                               gint *width, gint *height);

G_END_DECLS

#endif /* __NBTK_STYLABLE_H__ */
