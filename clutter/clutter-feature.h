/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By Matthew Allum  <mallum@openedhand.com>
 *
 * Copyright (C) 2006 OpenedHand
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
/**
 * SECTION:clutter-main
 * @short_description: Various 'global' clutter functions.
 *
 * Functions to retrieve various global Clutter resources and other utility
 * functions for mainloops, events and threads
 */

#ifndef _HAVE_CLUTTER_FEATURE_H
#define _HAVE_CLUTTER_FEATURE_H

#include <glib.h>

G_BEGIN_DECLS

/**
 * ClutterFeatureFlags:
 * @CLUTTER_FEATURE_TEXTURE_RECTANGLE: Set if NPOTS textures supported.
 * @CLUTTER_FEATURE_SYNC_TO_VBLANK: Set if vblank syncing supported.
 * @CLUTTER_FEATURE_TEXTURE_YUV: Set if YUV based textures supported.
 * @CLUTTER_FEATURE_TEXTURE_READ_PIXELS: Set if texture pixels can be read.
 * @CLUTTER_FEATURE_STAGE_STATIC: Set if stage size if fixed (i.e framebuffer)
 * @CLUTTER_FEATURE_STAGE_USER_RESIZE: Set if stage is able to be user resized.
 * @CLUTTER_FEATURE_STAGE_CURSOR: Set if stage has a graphical cursor.
 * @CLUTTER_FEATURE_SHADERS_GLSL: Set if the backend supports GLSL shaders.
 * @CLUTTER_FEATURE_OFFSCREEN: Set if the backend supports offscreen rendering.
 *
 * Runtime flags indicating specific features available via Clutter window
 * sysytem and graphics backend.
 *
 * Since: 0.4
 */
typedef enum 
{
  CLUTTER_FEATURE_TEXTURE_RECTANGLE      = (1 << 1),
  CLUTTER_FEATURE_SYNC_TO_VBLANK         = (1 << 2),
  CLUTTER_FEATURE_TEXTURE_YUV            = (1 << 3),
  CLUTTER_FEATURE_TEXTURE_READ_PIXELS    = (1 << 4),
  CLUTTER_FEATURE_STAGE_STATIC           = (1 << 5),
  CLUTTER_FEATURE_STAGE_USER_RESIZE      = (1 << 6),
  CLUTTER_FEATURE_STAGE_CURSOR           = (1 << 7),
  CLUTTER_FEATURE_SHADERS_GLSL           = (1 << 8),
  CLUTTER_FEATURE_OFFSCREEN              = (1 << 9)
} ClutterFeatureFlags;

gboolean            clutter_feature_available       (ClutterFeatureFlags feature);
ClutterFeatureFlags clutter_feature_get_all         (void);

G_END_DECLS

#endif

