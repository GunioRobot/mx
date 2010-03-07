/*
 * mx-deform-bowtie.h: A bowtie deformation actor
 *
 * Copyright 2010 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 * Boston, MA 02111-1307, USA.
 *
 * Written by: Chris Lord <chris@linux.intel.com>
 *
 */

#include "mx-deform-bowtie.h"
#include "mx-private.h"

#include <math.h>

G_DEFINE_TYPE (MxDeformBowtie, mx_deform_bowtie, MX_TYPE_DEFORM_TEXTURE)

#define DEFORM_BOWTIE_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), \
                                  MX_TYPE_DEFORM_BOWTIE, MxDeformBowtiePrivate))

struct _MxDeformBowtiePrivate
{
  gdouble period;
  ClutterTexture *back;
  gboolean flip_back;
  gulong back_id;
};

enum
{
  PROP_0,

  PROP_PERIOD,
  PROP_FLIP_BACK
};

static void
mx_deform_bowtie_texture_vflip (ClutterTexture *texture)
{
  CoglHandle material;

  material = clutter_texture_get_cogl_material (texture);

  if (material)
    {
      CoglMatrix matrix;
      cogl_matrix_init_identity (&matrix);

      /* Vflip */
      cogl_matrix_scale (&matrix, 1.f, -1.f, 1.f);
      cogl_matrix_translate (&matrix, 0.f, 1.f, 0.f);

      cogl_material_set_layer_matrix (material, 0, &matrix);
    }
}

static void
mx_deform_bowtie_texture_reset (ClutterTexture *texture)
{
  CoglHandle material;

  material = clutter_texture_get_cogl_material (texture);

  if (material)
    {
      CoglMatrix matrix;
      cogl_matrix_init_identity (&matrix);
      cogl_material_set_layer_matrix (material, 0, &matrix);
    }
}

static void
mx_deform_bowtie_get_property (GObject    *object,
                              guint       property_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  MxDeformBowtiePrivate *priv = MX_DEFORM_BOWTIE (object)->priv;

  switch (property_id)
    {
    case PROP_PERIOD:
      g_value_set_double (value, priv->period);
      break;

    case PROP_FLIP_BACK:
      g_value_set_boolean (value, priv->flip_back);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
mx_deform_bowtie_set_property (GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  gdouble set_value;
  gboolean flip_back;
  MxDeformTexture *texture = MX_DEFORM_TEXTURE (object);
  MxDeformBowtiePrivate *priv = MX_DEFORM_BOWTIE (object)->priv;

  switch (property_id)
    {
    case PROP_PERIOD:
      set_value = g_value_get_double (value);
      if (priv->period != set_value)
        {
          priv->period = set_value;
          g_object_notify (object, "period");
          mx_deform_texture_invalidate (texture);
        }
      break;

    case PROP_FLIP_BACK:
      flip_back = g_value_get_boolean (value);
      if (priv->flip_back != flip_back)
        {
          priv->flip_back = flip_back;
          if (priv->back)
            {
              if (priv->back_id)
                g_signal_handler_disconnect (priv->back, priv->back_id);

              if (flip_back)
                {
                  priv->back_id =
                    g_signal_connect (priv->back, "notify::cogl-texture",
                                      G_CALLBACK (
                                        mx_deform_bowtie_texture_vflip),
                                      texture);
                  mx_deform_bowtie_texture_vflip (priv->back);
                }
              else
                {
                  mx_deform_bowtie_texture_reset (priv->back);
                  priv->back_id = 0;
                }
            }
        }

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
mx_deform_bowtie_deform (MxDeformTexture   *texture,
                         CoglTextureVertex *vertex,
                         gfloat             width,
                         gfloat             height)
{
  gfloat cx, cy, rx, ry, turn_angle, height_radius;
  guint shade;

  MxDeformBowtiePrivate *priv = ((MxDeformBowtie *)texture)->priv;

  cx = priv->period * (width + width/2);
  cy = height/2;

  rx = ((vertex->x - cx) * cos (0)) -
       ((vertex->y - cy) * sin (0));
  ry = ((vertex->x - cx) * sin (0)) +
       ((vertex->y - cy) * cos (0));

  /* Make angle as a function of distance from the curl ray */
  turn_angle = MAX (-G_PI, MIN (0, (rx / (width/4)) * G_PI_2));

  /* Add a gradient that makes it look like lighting */
  shade = (cos (turn_angle * 2) * 96) + 159;
  vertex->color.red = shade;
  vertex->color.green = shade;
  vertex->color.blue = shade;

  /* Calculate the point on a cone (note, a cone, not a right cone) */
  height_radius = ry;
  /*ClutterFixed height_radius =
    clutter_qmulx (clutter_qdivx (ry, height/2), height/2);*/

  ry = height_radius * cos (turn_angle);
  vertex->x = (rx * cos (0)) - (ry * sin (0)) + cx;
  vertex->y = (rx * sin (0)) + (ry * cos (0)) + cy;
  vertex->z = height_radius * sin (turn_angle);
}

static void
mx_deform_bowtie_back_notify (MxDeformBowtie *self,
                              GParamSpec     *pspec)
{
  MxDeformBowtiePrivate *priv = self->priv;

  if (priv->back_id && priv->back)
    {
      g_signal_handler_disconnect (priv->back, priv->back_id);
      priv->back_id = 0;
    }

  if (priv->back)
    {
      if (priv->flip_back)
        mx_deform_bowtie_texture_reset (priv->back);

      g_object_remove_weak_pointer (G_OBJECT (priv->back),
                                    (gpointer *)&priv->back);
      priv->back = NULL;
    }

  mx_deform_texture_get_textures (MX_DEFORM_TEXTURE (self), NULL, &priv->back);

  if (priv->back)
    {
      g_object_add_weak_pointer (G_OBJECT (priv->back),
                                 (gpointer)&priv->back);

      if (priv->flip_back)
        {
          priv->back_id =
            g_signal_connect (priv->back, "notify::cogl-texture",
                              G_CALLBACK (mx_deform_bowtie_texture_vflip),
                              self);
          mx_deform_bowtie_texture_vflip (priv->back);
        }
    }
}

static void
mx_deform_bowtie_dispose (GObject *object)
{
  MxDeformBowtiePrivate *priv = MX_DEFORM_BOWTIE (object)->priv;

  if (priv->back_id && priv->back)
    {
      g_signal_handler_disconnect (priv->back, priv->back_id);
      priv->back_id = 0;
    }

  if (priv->back)
    {
      CoglHandle material;

      /* Reset layer matrix */
      material = clutter_texture_get_cogl_material (priv->back);

      if (material)
        {
          CoglMatrix matrix;
          cogl_matrix_init_identity (&matrix);
          cogl_material_set_layer_matrix (material, 0, &matrix);
        }

      g_object_remove_weak_pointer (G_OBJECT (priv->back),
                                    (gpointer *)&priv->back);
      priv->back = NULL;
    }

  G_OBJECT_CLASS (mx_deform_bowtie_parent_class)->dispose (object);
}

static void
mx_deform_bowtie_class_init (MxDeformBowtieClass *klass)
{
  GParamSpec *pspec;

  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  MxDeformTextureClass *deform_class = MX_DEFORM_TEXTURE_CLASS (klass);

  g_type_class_add_private (klass, sizeof (MxDeformBowtiePrivate));

  object_class->get_property = mx_deform_bowtie_get_property;
  object_class->set_property = mx_deform_bowtie_set_property;
  object_class->dispose = mx_deform_bowtie_dispose;

  deform_class->deform = mx_deform_bowtie_deform;

  pspec = g_param_spec_double ("period",
                               "Period",
                               "Effect period",
                               0.0, 1.0, 0.0,
                               MX_PARAM_READWRITE);
  g_object_class_install_property (object_class, PROP_PERIOD, pspec);

  pspec = g_param_spec_boolean ("flip-back",
                                "Flip back-face",
                                "Apply a vertical flip transformation to the "
                                "back face.",
                                TRUE,
                                MX_PARAM_READWRITE);
  g_object_class_install_property (object_class, PROP_PERIOD, pspec);
}

static void
mx_deform_bowtie_init (MxDeformBowtie *self)
{
  self->priv = DEFORM_BOWTIE_PRIVATE (self);
  self->priv->flip_back = TRUE;
  g_signal_connect (self, "notify::back",
                    G_CALLBACK (mx_deform_bowtie_back_notify), NULL);
}

ClutterActor *
mx_deform_bowtie_new (void)
{
  return g_object_new (MX_TYPE_DEFORM_BOWTIE, NULL);
}
