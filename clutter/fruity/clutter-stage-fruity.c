#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clutter-stage-fruity.h"
#include "clutter-fruity.h"
#include "clutter-backend-fruity.h"

#include "../clutter-main.h"
#include "../clutter-feature.h"
#include "../clutter-color.h"
#include "../clutter-util.h"
#include "../clutter-event.h"
#include "../clutter-enum-types.h"
#include "../clutter-private.h"
#include "../clutter-debug.h"
#include "../clutter-units.h"
#include "../clutter-stage.h"
#include "../clutter-stage-window.h"

static void clutter_stage_window_iface_init (ClutterStageWindowIface *iface);

G_DEFINE_TYPE_WITH_CODE (ClutterStageEGL,
                         clutter_stage_egl,
                         CLUTTER_TYPE_ACTOR,
                         G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_STAGE_WINDOW,
                                                clutter_stage_window_iface_init));

static void
clutter_stage_egl_show (ClutterActor *actor)
{
    ClutterStageEGL *stage_egl = CLUTTER_STAGE_EGL (actor);

  CLUTTER_ACTOR_SET_FLAGS (stage_egl, CLUTTER_ACTOR_MAPPED);
  CLUTTER_ACTOR_SET_FLAGS (stage_egl->wrapper, CLUTTER_ACTOR_MAPPED);

  CLUTTER_ACTOR_CLASS (clutter_stage_egl_parent_class)->show (actor);
}

static void
clutter_stage_egl_hide (ClutterActor *actor)
{
    ClutterStageEGL *stage_egl = CLUTTER_STAGE_EGL (actor);

  CLUTTER_ACTOR_UNSET_FLAGS (stage_egl, CLUTTER_ACTOR_MAPPED);
  CLUTTER_ACTOR_UNSET_FLAGS (stage_egl->wrapper, CLUTTER_ACTOR_MAPPED);
  CLUTTER_ACTOR_CLASS (clutter_stage_egl_parent_class)->hide (actor);
}

static void
clutter_stage_egl_unrealize (ClutterActor *actor)
{
  ClutterStageEGL *stage_egl = CLUTTER_STAGE_EGL (actor);

  CLUTTER_MARK();

  CLUTTER_ACTOR_CLASS (clutter_stage_egl_parent_class)->unrealize (actor);

  if (stage_egl->egl_surface)
    {
      eglDestroySurface (clutter_egl_display (), stage_egl->egl_surface);
      stage_egl->egl_surface = EGL_NO_SURFACE;
    }

  clutter_stage_ensure_current (stage_egl->wrapper);
}

static void
clutter_stage_egl_realize (ClutterActor *actor)
{
  ClutterStageEGL     *stage_egl = CLUTTER_STAGE_EGL (actor);
  ClutterBackendEGL   *backend_egl;
  EGLConfig            configs[2];
  EGLint               config_count;
  EGLBoolean           status;
  gboolean             is_offscreen;

  CLUTTER_NOTE (BACKEND, "Realizing main stage");

  g_object_get (stage_egl->wrapper, "offscreen", &is_offscreen, NULL);

  backend_egl = CLUTTER_BACKEND_EGL (clutter_get_default_backend ());

  if (G_LIKELY (!is_offscreen))
    {
      EGLint cfg_attribs[] = { EGL_BUFFER_SIZE,    EGL_DONT_CARE,
			       EGL_RED_SIZE,       5,
			       EGL_GREEN_SIZE,     6,
			       EGL_BLUE_SIZE,      5,
			       EGL_DEPTH_SIZE,     16,
			       EGL_ALPHA_SIZE,     EGL_DONT_CARE,
			       EGL_STENCIL_SIZE,   2,
			       EGL_SURFACE_TYPE,   EGL_WINDOW_BIT,
			       EGL_NONE };

      status = eglGetConfigs (backend_egl->edpy,
			      configs, 
			      2, 
			      &config_count);

      if (status != EGL_TRUE)
	g_warning ("eglGetConfigs failed");

      status = eglChooseConfig (backend_egl->edpy,
				cfg_attribs,
				configs,
                                G_N_ELEMENTS (configs),
				&config_count);

      if (status != EGL_TRUE)
	g_warning ("eglChooseConfig failed");

      if (stage_egl->egl_surface != EGL_NO_SURFACE)
        {
	  eglDestroySurface (backend_egl->edpy, stage_egl->egl_surface);
          stage_egl->egl_surface = EGL_NO_SURFACE;
        }

       if (backend_egl->egl_context)
         {
            eglDestroyContext (backend_egl->edpy, backend_egl->egl_context);
            backend_egl->egl_context = NULL;
         }

      stage_egl->egl_surface =
	eglCreateWindowSurface (backend_egl->edpy,
                                configs[0],
                                NULL,
                                NULL);



      if (stage_egl->egl_surface == EGL_NO_SURFACE)
        {
	  g_critical ("Unable to create an EGL surface");

          CLUTTER_ACTOR_UNSET_FLAGS (actor, CLUTTER_ACTOR_REALIZED);
          return;
        }

      eglQuerySurface (backend_egl->edpy,
		       stage_egl->egl_surface,
		       EGL_WIDTH,
		       &stage_egl->surface_width);

      eglQuerySurface (backend_egl->edpy,
		       stage_egl->egl_surface,
		       EGL_HEIGHT,
		       &stage_egl->surface_height);

      CLUTTER_NOTE (BACKEND, "EGL surface is %ix%i", 
		    stage_egl->surface_width,
                    stage_egl->surface_height);


      if (G_UNLIKELY (backend_egl->egl_context == NULL))
        {
          CLUTTER_NOTE (GL, "Creating EGL Context");

          backend_egl->egl_context = eglCreateContext (backend_egl->edpy,
                                                       configs[0],
                                                       EGL_NO_CONTEXT,
                                                       NULL);

          if (backend_egl->egl_context == EGL_NO_CONTEXT)
            {
              g_critical ("Unable to create a suitable EGL context");

              CLUTTER_ACTOR_UNSET_FLAGS (actor, CLUTTER_ACTOR_REALIZED);
              return;
            }
        }

      /* this will make sure to set the current context */
      CLUTTER_NOTE (BACKEND, "Marking stage as realized and setting context");
      CLUTTER_ACTOR_SET_FLAGS (stage_egl->wrapper, CLUTTER_ACTOR_REALIZED);
      CLUTTER_ACTOR_SET_FLAGS (stage_egl, CLUTTER_ACTOR_REALIZED);


      status = eglMakeCurrent (backend_egl->edpy,
                               stage_egl->egl_surface,
                               stage_egl->egl_surface,
                               backend_egl->egl_context);

      if (status != EGL_TRUE)
          g_warning ("eglMakeCurrent");   

      /*clutter_stage_ensure_current (stage_egl->wrapper);*/
    }
  else
    {
      g_warning("EGL Backend does not yet support offscreen rendering\n");
      CLUTTER_ACTOR_UNSET_FLAGS (actor, CLUTTER_ACTOR_REALIZED);
      return;
    }


  CLUTTER_SET_PRIVATE_FLAGS (stage_egl->wrapper, CLUTTER_ACTOR_SYNC_MATRICES);
}

static void
clutter_stage_egl_query_coords (ClutterActor    *self,
                                ClutterActorBox *box)
{
  ClutterStageEGL *stage_egl = CLUTTER_STAGE_EGL (self);

  box->x1 = box->y1 = 0;
  box->x2 = box->x1 + CLUTTER_UNITS_FROM_INT (stage_egl->surface_width);
  box->y2 = box->y1 + CLUTTER_UNITS_FROM_INT (stage_egl->surface_height);
}

static void
clutter_stage_egl_request_coords (ClutterActor    *self,
				  ClutterActorBox *box)
{
  ClutterStageEGL *stage_egl = CLUTTER_STAGE_EGL (self);

  /* framebuffer no resize */
  box->x1 = 0;
  box->y1 = 0;
  box->x2 = CLUTTER_UNITS_FROM_INT (stage_egl->surface_width);
  box->y2 = CLUTTER_UNITS_FROM_INT (stage_egl->surface_height);

  CLUTTER_ACTOR_CLASS (clutter_stage_egl_parent_class)->request_coords (self,
                                                                        box);
}

static void
clutter_stage_egl_dispose (GObject *gobject)
{
  ClutterStageEGL *stage_egl = CLUTTER_STAGE_EGL (gobject);

  clutter_actor_unrealize (CLUTTER_ACTOR (stage_egl));

  G_OBJECT_CLASS (clutter_stage_egl_parent_class)->dispose (gobject);
}

static void
clutter_stage_egl_class_init (ClutterStageEGLClass *klass)
{
  GObjectClass      *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class   = CLUTTER_ACTOR_CLASS (klass);

  gobject_class->dispose = clutter_stage_egl_dispose;

  actor_class->show           = clutter_stage_egl_show;
  actor_class->hide           = clutter_stage_egl_hide;
  actor_class->realize        = clutter_stage_egl_realize;
  actor_class->unrealize      = clutter_stage_egl_unrealize;
  actor_class->request_coords = clutter_stage_egl_request_coords;
  actor_class->query_coords   = clutter_stage_egl_query_coords;
}

static void
clutter_stage_egl_set_fullscreen (ClutterStageWindow *stage_window,
                                  gboolean            fullscreen)
{
  g_warning ("Stage of type `%s' do not support ClutterStage::set_fullscreen",
             G_OBJECT_TYPE_NAME (stage_window));
}

static ClutterActor *
clutter_stage_egl_get_wrapper (ClutterStageWindow *stage_window)
{
  return CLUTTER_ACTOR (CLUTTER_STAGE_EGL (stage_window)->wrapper);
}

static void
clutter_stage_window_iface_init (ClutterStageWindowIface *iface)
{
  iface->set_fullscreen = clutter_stage_egl_set_fullscreen;
  iface->set_title = NULL;
  iface->get_wrapper = clutter_stage_egl_get_wrapper;
}

static void
clutter_stage_egl_init (ClutterStageEGL *stage)
{
}