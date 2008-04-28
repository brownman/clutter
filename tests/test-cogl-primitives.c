#include <config.h>
#include <glib.h>
#include <stdlib.h>
#include <clutter/clutter.h>
#include <cogl/cogl.h>

/* Coglbox declaration
 *--------------------------------------------------*/

G_BEGIN_DECLS
  
#define TEST_TYPE_COGLBOX test_coglbox_get_type()

#define TEST_COGLBOX(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  TEST_TYPE_COGLBOX, TestCoglboxClass))

#define TEST_COGLBOX_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  TEST_TYPE_COGLBOX, TestCoglboxClass))

#define TEST_IS_COGLBOX(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  TEST_TYPE_COGLBOX))

#define TEST_IS_COGLBOX_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  TEST_TYPE_COGLBOX))

#define TEST_COGLBOX_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  TEST_TYPE_COGLBOX, TestCoglboxClass))

typedef struct _TestCoglbox        TestCoglbox;
typedef struct _TestCoglboxClass   TestCoglboxClass;
typedef struct _TestCoglboxPrivate TestCoglboxPrivate;

struct _TestCoglbox
{
  ClutterActor           parent;

  /*< private >*/
  TestCoglboxPrivate *priv;
};

struct _TestCoglboxClass 
{
  ClutterActorClass parent_class;

  /* padding for future expansion */
  void (*_test_coglbox1) (void);
  void (*_test_coglbox2) (void);
  void (*_test_coglbox3) (void);
  void (*_test_coglbox4) (void);
};

GType test_coglbox_get_type (void) G_GNUC_CONST;

G_END_DECLS

/* Coglbox private declaration
 *--------------------------------------------------*/

G_DEFINE_TYPE (TestCoglbox, test_coglbox, CLUTTER_TYPE_ACTOR);

#define TEST_COGLBOX_GET_PRIVATE(obj) \
(G_TYPE_INSTANCE_GET_PRIVATE ((obj), TEST_TYPE_COGLBOX, TestCoglboxPrivate))

struct _TestCoglboxPrivate
{
  void (*_test_coglbox_priv1) (void);
};

/* Coglbox implementation
 *--------------------------------------------------*/

typedef void (*PaintFunc) (void);

static void
test_paint_line ()
{
  cogl_line (CLUTTER_INT_TO_FIXED (-50),
	     CLUTTER_INT_TO_FIXED (-25),
	     CLUTTER_INT_TO_FIXED (50),
	     CLUTTER_INT_TO_FIXED (25));
}

static void
test_paint_rect ()
{
  cogl_rectangle (CLUTTER_INT_TO_FIXED (-50),
		  CLUTTER_INT_TO_FIXED (-25),
		  CLUTTER_INT_TO_FIXED (100),
		  CLUTTER_INT_TO_FIXED (50));
}

static void
test_paint_rndrect()
{
  cogl_round_rectangle (CLUTTER_INT_TO_FIXED (-50),
			CLUTTER_INT_TO_FIXED (-25),
			CLUTTER_INT_TO_FIXED (100),
			CLUTTER_INT_TO_FIXED (50),
			CLUTTER_INT_TO_FIXED (10),
			5);
}

static void
test_paint_polyl ()
{
  ClutterFixed poly_coords[] = {
    CLUTTER_INT_TO_FIXED (-50),
    CLUTTER_INT_TO_FIXED (-50),
    CLUTTER_INT_TO_FIXED (+50),
    CLUTTER_INT_TO_FIXED (-30),
    CLUTTER_INT_TO_FIXED (+30),
    CLUTTER_INT_TO_FIXED (+30),
    CLUTTER_INT_TO_FIXED (-30),
    CLUTTER_INT_TO_FIXED (+40)
  };
  
  cogl_polyline (poly_coords, 4);
}

static void
test_paint_polyg ()
{
  gint poly_coords[] = {
    CLUTTER_INT_TO_FIXED (-50),
    CLUTTER_INT_TO_FIXED (-50),
    CLUTTER_INT_TO_FIXED (+50),
    CLUTTER_INT_TO_FIXED (-30),
    CLUTTER_INT_TO_FIXED (+30),
    CLUTTER_INT_TO_FIXED (+30),
    CLUTTER_INT_TO_FIXED (-30),
    CLUTTER_INT_TO_FIXED (+40)
  };
  
  cogl_polygon (poly_coords, 4);
}

static void
test_paint_arc ()
{
  cogl_arc (0,0,
	    CLUTTER_INT_TO_FIXED (60),
	    CLUTTER_INT_TO_FIXED (40),
	    CLUTTER_ANGLE_FROM_DEG (-45),
	    CLUTTER_ANGLE_FROM_DEG (+45),
	    10);
}

static void
test_paint_elp ()
{
  cogl_ellipse (0, 0,
		CLUTTER_INT_TO_FIXED (60),
		CLUTTER_INT_TO_FIXED (40),
		10);
}

static void
test_paint_bezier2 ()
{
  cogl_path_move_to (CLUTTER_INT_TO_FIXED (-50),
		     CLUTTER_INT_TO_FIXED (+25));
  
  cogl_path_bezier2_to (CLUTTER_INT_TO_FIXED (0),
			CLUTTER_INT_TO_FIXED (-25),
			CLUTTER_INT_TO_FIXED (+50),
			CLUTTER_INT_TO_FIXED (+25));
}

static void
test_paint_bezier3 ()
{
  cogl_path_move_to (CLUTTER_INT_TO_FIXED (-50),
		     CLUTTER_INT_TO_FIXED (+50));
  
  cogl_path_bezier3_to (CLUTTER_INT_TO_FIXED (+100),
			CLUTTER_INT_TO_FIXED (-50),
			CLUTTER_INT_TO_FIXED (-100),
			CLUTTER_INT_TO_FIXED (-50),
			CLUTTER_INT_TO_FIXED (+50),
			CLUTTER_INT_TO_FIXED (+50));
}

static void
test_coglbox_paint(ClutterActor *self)
{
  TestCoglboxPrivate *priv;
  
  ClutterColor cfill;
  ClutterColor cstroke;
  
  static GTimer *timer = NULL;
  static gint paint_index = 0;
  
  const gint NUM_PAINT_FUNCS = 9;
  PaintFunc paint_func [NUM_PAINT_FUNCS];
  
  priv = TEST_COGLBOX_GET_PRIVATE (self);
  
  paint_func[0] = test_paint_line;
  paint_func[1] = test_paint_rect;
  paint_func[2] = test_paint_rndrect;
  paint_func[3] = test_paint_polyl;
  paint_func[4] = test_paint_polyg;
  paint_func[5] = test_paint_arc;
  paint_func[6] = test_paint_elp;
  paint_func[7] = test_paint_bezier2;
  paint_func[8] = test_paint_bezier3;
  
  if (!timer)
    {
      timer = g_timer_new ();
      g_timer_start (timer);
    }
  
  if (g_timer_elapsed (timer, NULL) >= 1)
    {
      paint_index = (++paint_index) % NUM_PAINT_FUNCS;
      g_timer_start (timer);
    }
  
  cfill.red    = 0;
  cfill.green  = 160;
  cfill.blue   = 0;
  cfill.alpha  = 255;
  
  cstroke.red    = 200;
  cstroke.green  = 0;
  cstroke.blue   = 0;
  cstroke.alpha  = 255;
  
  cogl_push_matrix ();
  
  paint_func[paint_index] ();
  
  cogl_translate (100,100,0);
  cogl_color (&cstroke);
  cogl_stroke ();
  
  cogl_translate (150,0,0);
  cogl_color (&cfill);
  cogl_fill ();
  
  cogl_pop_matrix();
}

static void
test_coglbox_finalize (GObject *object)
{
  G_OBJECT_CLASS (test_coglbox_parent_class)->finalize (object);
}

static void
test_coglbox_dispose (GObject *object)
{
  TestCoglboxPrivate *priv;
  
  priv = TEST_COGLBOX_GET_PRIVATE (object);
  
  G_OBJECT_CLASS (test_coglbox_parent_class)->dispose (object);
}

static void
test_coglbox_init (TestCoglbox *self)
{
  TestCoglboxPrivate *priv;
  self->priv = priv = TEST_COGLBOX_GET_PRIVATE(self);
}

static void
test_coglbox_class_init (TestCoglboxClass *klass)
{
  GObjectClass      *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class   = CLUTTER_ACTOR_CLASS (klass);

  gobject_class->finalize     = test_coglbox_finalize;
  gobject_class->dispose      = test_coglbox_dispose;  
  actor_class->paint          = test_coglbox_paint;
  
  g_type_class_add_private (gobject_class, sizeof (TestCoglboxPrivate));
}

ClutterActor*
test_coglbox_new (void)
{
  return g_object_new (TEST_TYPE_COGLBOX, NULL);
}

#define SPIN()   while (g_main_context_pending (NULL)) \
                     g_main_context_iteration (NULL, FALSE);

int
main (int argc, char *argv[])
{
  ClutterActor *stage;
  ClutterActor *coglbox;
  
  clutter_init(&argc, &argv);
  
  stage = clutter_stage_get_default ();
  clutter_actor_set_size (stage, 400, 400);
  clutter_stage_set_title (CLUTTER_STAGE (stage), "Cogl Test");
  
  coglbox = test_coglbox_new ();
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), coglbox);
  
  clutter_actor_show_all (stage);
  
  while (1)
    {
      clutter_actor_hide (coglbox);
      clutter_actor_show (coglbox);
      SPIN();
    }
  
  return 0;
}