#include <clutter/clutter.h>

int
main (int argc, char *argv[])
{
  ClutterTimeline  *timeline;
  ClutterAlpha     *alpha;
  ClutterBehaviour *o_behave, *p_behave;
  ClutterActor     *stage;
  ClutterActor     *group, *rect, *hand;
  ClutterColor      stage_color = { 0xcc, 0xcc, 0xcc, 0xff };
  ClutterColor      rect_bg_color = { 0x33, 0x22, 0x22, 0xff };
  ClutterColor      rect_border_color = { 0, 0, 0, 0 };
  GdkPixbuf        *pixbuf;

  ClutterKnot       knots[] = {{ 0, 0 }, { 0, 300 }, { 300, 300 },
                               { 300, 0 }, {0, 0 }};

  clutter_init (&argc, &argv);

  stage = clutter_stage_get_default ();
  g_signal_connect (stage, "key-press-event",
                    G_CALLBACK (clutter_main_quit),
                    NULL);

  pixbuf = gdk_pixbuf_new_from_file ("redhand.png", NULL);

  if (!pixbuf)
    g_error("pixbuf load failed");

  clutter_stage_set_color (CLUTTER_STAGE (stage),
		           &stage_color);

  /* Make a hand */
  group = clutter_group_new ();
  clutter_group_add (CLUTTER_GROUP (stage), group);
  clutter_actor_show (group);
  
  rect = clutter_rectangle_new ();
  clutter_actor_set_position (rect, 0, 0);
  clutter_actor_set_size (rect,
                          gdk_pixbuf_get_width (pixbuf),
                          gdk_pixbuf_get_height (pixbuf));
  clutter_rectangle_set_color (CLUTTER_RECTANGLE (rect),
                               &rect_bg_color);
  clutter_rectangle_set_border_width (CLUTTER_RECTANGLE (rect), 10);
  clutter_color_parse ("DarkSlateGray", &rect_border_color);
  clutter_rectangle_set_border_color (CLUTTER_RECTANGLE (rect),
                                      &rect_border_color);
  clutter_actor_show (rect);

  hand = clutter_texture_new_from_pixbuf (pixbuf);
  clutter_actor_set_position (hand, 0, 0);
  clutter_actor_show (hand);

  clutter_group_add_many (CLUTTER_GROUP (group), rect, hand, NULL);

  /* Make a timeline */
  timeline = clutter_timeline_new (100, 26); /* num frames, fps */
  g_object_set (timeline, "loop", TRUE, 0);  

  /* Set an alpha func to power behaviour - ramp is constant rise/fall */
  alpha = clutter_alpha_new_full (timeline,
                                  CLUTTER_ALPHA_RAMP,
                                  NULL, NULL);

  /* Create a behaviour for that alpha */
  o_behave = clutter_behaviour_opacity_new (alpha, 0X33, 0xff); 

  /* Apply it to our actor */
  clutter_behaviour_apply (o_behave, group);

  /* Make a path behaviour and apply that too */
  p_behave = clutter_behaviour_path_new (alpha, knots, 5); 
  clutter_behaviour_apply (p_behave, group);

  /* start the timeline and thus the animations */
  clutter_timeline_start (timeline);

  clutter_group_show_all (CLUTTER_GROUP (stage));

  clutter_main();

  g_object_unref (o_behave);
  g_object_unref (p_behave);

  return 0;
}
