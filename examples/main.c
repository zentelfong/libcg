#include <cat.h>
#include <cg.h>

static void cg_surface_write_to_png(struct cg_surface_t * surface, const char * filename)
{
	cg_surface_save_file(surface,filename);
}

static void test_arc(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	double xc = 128.0;
	double yc = 128.0;
	double radius = 100.0;
	double angle1 = 45.0 * (M_PI / 180.0);
	double angle2 = 180.0 * (M_PI / 180.0);

	cg_set_line_width(ctx, 10.0);
	cg_arc(ctx, xc, yc, radius, angle1, angle2);
	cg_stroke(ctx);

	cg_set_source_rgba(ctx, 1, 0.2, 0.2, 0.6);
	cg_set_line_width(ctx, 6.0);

	cg_arc(ctx, xc, yc, 10.0, 0, 2 * M_PI);
	cg_fill(ctx);

	cg_arc(ctx, xc, yc, radius, angle1, angle1);
	cg_line_to(ctx, xc, yc);
	cg_arc(ctx, xc, yc, radius, angle2, angle2);
	cg_line_to(ctx, xc, yc);
	cg_stroke(ctx);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_arc_negative(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	double xc = 128.0;
	double yc = 128.0;
	double radius = 100.0;
	double angle1 = 45.0 * (M_PI / 180.0);
	double angle2 = 180.0 * (M_PI / 180.0);

	cg_set_line_width(ctx, 10.0);
	cg_arc_negative(ctx, xc, yc, radius, angle1, angle2);
	cg_stroke(ctx);

	cg_set_source_rgba(ctx, 1, 0.2, 0.2, 0.6);
	cg_set_line_width(ctx, 6.0);

	cg_arc(ctx, xc, yc, 10.0, 0, 2 * M_PI);
	cg_fill(ctx);

	cg_arc(ctx, xc, yc, radius, angle1, angle1);
	cg_line_to(ctx, xc, yc);
	cg_arc(ctx, xc, yc, radius, angle2, angle2);
	cg_line_to(ctx, xc, yc);
	cg_stroke(ctx);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_clip(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	cg_arc(ctx, 128.0, 128.0, 76.8, 0, 2 * M_PI);
	cg_clip(ctx);

	cg_new_path(ctx);
	cg_rectangle(ctx, 0, 0, 256, 256);
	cg_fill(ctx);
	cg_set_source_rgb(ctx, 0, 1, 0);
	cg_move_to(ctx, 0, 0);
	cg_line_to(ctx, 256, 256);
	cg_move_to(ctx, 256, 0);
	cg_line_to(ctx, 0, 256);
	cg_set_line_width(ctx, 10.0);
	cg_stroke(ctx);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_clip_image(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	struct cg_surface_t *  img = cg_surface_create_for_data(128, 128, (void *)cat_img_128_128);
	cg_arc(ctx, 128.0, 128.0, 76.8, 0, 2 * M_PI);
	cg_clip(ctx);
	cg_scale(ctx, 256.0 / img->width, 256.0 / img->height);
	cg_set_source_surface(ctx, img, 0, 0);
	cg_paint(ctx);
	cg_surface_destroy(img);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_curve_rectangle(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	double x0 = 25.6,
		y0 = 25.6,
		rect_width = 204.8,
		rect_height = 204.8,
		radius = 102.4;
	double x1, y1;

	x1 = x0 + rect_width;
	y1 = y0 + rect_height;
	if(!rect_width || !rect_height)
		return;
	if(rect_width / 2 < radius)
	{
		if(rect_height / 2 < radius)
		{
			cg_move_to(ctx, x0, (y0 + y1) / 2);
			cg_curve_to(ctx, x0, y0, x0, y0, (x0 + x1) / 2, y0);
			cg_curve_to(ctx, x1, y0, x1, y0, x1, (y0 + y1) / 2);
			cg_curve_to(ctx, x1, y1, x1, y1, (x1 + x0) / 2, y1);
			cg_curve_to(ctx, x0, y1, x0, y1, x0, (y0 + y1) / 2);
		}
		else
		{
			cg_move_to(ctx, x0, y0 + radius);
			cg_curve_to(ctx, x0, y0, x0, y0, (x0 + x1) / 2, y0);
			cg_curve_to(ctx, x1, y0, x1, y0, x1, y0 + radius);
			cg_line_to(ctx, x1, y1 - radius);
			cg_curve_to(ctx, x1, y1, x1, y1, (x1 + x0) / 2, y1);
			cg_curve_to(ctx, x0, y1, x0, y1, x0, y1 - radius);
		}
	}
	else
	{
		if(rect_height / 2 < radius)
		{
			cg_move_to(ctx, x0, (y0 + y1) / 2);
			cg_curve_to(ctx, x0, y0, x0, y0, x0 + radius, y0);
			cg_line_to(ctx, x1 - radius, y0);
			cg_curve_to(ctx, x1, y0, x1, y0, x1, (y0 + y1) / 2);
			cg_curve_to(ctx, x1, y1, x1, y1, x1 - radius, y1);
			cg_line_to(ctx, x0 + radius, y1);
			cg_curve_to(ctx, x0, y1, x0, y1, x0, (y0 + y1) / 2);
		}
		else
		{
			cg_move_to(ctx, x0, y0 + radius);
			cg_curve_to(ctx, x0, y0, x0, y0, x0 + radius, y0);
			cg_line_to(ctx, x1 - radius, y0);
			cg_curve_to(ctx, x1, y0, x1, y0, x1, y0 + radius);
			cg_line_to(ctx, x1, y1 - radius);
			cg_curve_to(ctx, x1, y1, x1, y1, x1 - radius, y1);
			cg_line_to(ctx, x0 + radius, y1);
			cg_curve_to(ctx, x0, y1, x0, y1, x0, y1 - radius);
		}
	}
	cg_close_path(ctx);

	cg_set_source_rgb(ctx, 0.5, 0.5, 1);
	cg_fill_preserve(ctx);
	cg_set_source_rgba(ctx, 0.5, 0, 0, 0.5);
	cg_set_line_width(ctx, 10.0);
	cg_stroke(ctx);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_curve_to(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	double x = 25.6, y = 128.0;
	double x1 = 102.4, y1 = 230.4, x2 = 153.6, y2 = 25.6, x3 = 230.4, y3 = 128.0;

	cg_move_to(ctx, x, y);
	cg_curve_to(ctx, x1, y1, x2, y2, x3, y3);

	cg_set_line_width(ctx, 10.0);
	cg_stroke(ctx);

	cg_set_source_rgba(ctx, 1, 0.2, 0.2, 0.6);
	cg_set_line_width(ctx, 6.0);
	cg_move_to(ctx, x, y);
	cg_line_to(ctx, x1, y1);
	cg_move_to(ctx, x2, y2);
	cg_line_to(ctx, x3, y3);
	cg_stroke(ctx);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_dash(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	double dashes[] = { 50.0, 10.0, 10.0, 10.0 };
	int ndash = sizeof(dashes) / sizeof(dashes[0]);
	double offset = 30.0;
	cg_set_dash(ctx, dashes, ndash, offset);
	cg_set_line_width(ctx, 10.0);
	cg_move_to(ctx, 128.0, 25.6);
	cg_line_to(ctx, 230.4, 230.4);
	cg_rel_line_to(ctx, -102.4, 0.0);
	cg_curve_to(ctx, 51.2, 230.4, 51.2, 128.0, 128.0, 128.0);
	cg_stroke(ctx);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_fill_and_stroke(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	cg_move_to(ctx, 128.0, 25.6);
	cg_line_to(ctx, 230.4, 230.4);
	cg_rel_line_to(ctx, -102.4, 0.0);
	cg_curve_to(ctx, 51.2, 230.4, 51.2, 128.0, 128.0, 128.0);
	cg_close_path(ctx);

	cg_move_to(ctx, 64.0, 25.6);
	cg_rel_line_to(ctx, 51.2, 51.2);
	cg_rel_line_to(ctx, -51.2, 51.2);
	cg_rel_line_to(ctx, -51.2, -51.2);
	cg_close_path(ctx);

	cg_set_line_width(ctx, 10.0);
	cg_set_source_rgb(ctx, 0, 0, 1);
	cg_fill_preserve(ctx);
	cg_set_source_rgb(ctx, 0, 0, 0);
	cg_stroke(ctx);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_fill_style(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	cg_set_line_width(ctx, 6);

	cg_rectangle(ctx, 12, 12, 232, 70);
	cg_circle(ctx, 64, 64, 40);
	cg_circle(ctx, 192, 64, 40);
	cg_set_fill_rule(ctx, CG_FILL_RULE_EVEN_ODD);
	cg_set_source_rgb(ctx, 0, 0.7, 0);
	cg_fill_preserve(ctx);
	cg_set_source_rgb(ctx, 0, 0, 0);
	cg_stroke(ctx);

	cg_save(ctx);
	cg_translate(ctx, 0, 128);
	cg_rectangle(ctx, 12, 12, 232, 70);
	cg_circle(ctx, 64, 64, 40);
	cg_circle(ctx, 192, 64, 40);
	cg_set_fill_rule(ctx, CG_FILL_RULE_NON_ZERO);
	cg_set_source_rgb(ctx, 0, 0, 0.9);
	cg_fill_preserve(ctx);
	cg_set_source_rgb(ctx, 0, 0, 0);
	cg_stroke(ctx);
	cg_restore(ctx);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_gradient(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	struct cg_gradient_t * grad;
	grad = cg_gradient_create_linear(0.0, 0.0, 0.0, 256.0);
	cg_gradient_add_stop_rgba(grad, 0, 1, 1, 1, 1);
	cg_gradient_add_stop_rgba(grad, 1, 0, 0, 0, 1);
	cg_rectangle(ctx, 0, 0, 256, 256);
	cg_set_source_gradient(ctx, grad);
	cg_fill(ctx);
	cg_gradient_destroy(grad);

	grad = cg_gradient_create_radial(15.2, 12.4, 25.6, 102.4, 102.4, 128.0);
	cg_gradient_add_stop_rgba(grad, 0, 1, 1, 0, 1);
	cg_gradient_add_stop_rgba(grad, 1, 1, 0, 0, 1);
	cg_set_source_gradient(ctx, grad);
	cg_arc(ctx, 128.0, 128.0, 76.8, 0, 2 * M_PI);
	cg_fill(ctx);
	cg_gradient_destroy(grad);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_image(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	struct cg_surface_t * img = cg_surface_create_for_data(128, 128, (void *)cat_img_128_128);
	cg_translate(ctx, 128.0, 128.0);
	cg_rotate(ctx, 45 * M_PI / 180);
	cg_scale(ctx, 256.0 / img->width, 256.0 / img->height);
	cg_translate(ctx, -0.5 * img->width, -0.5 * img->height);
	cg_set_source_surface(ctx, img, 0, 0);
	cg_paint(ctx);
	cg_surface_destroy(img);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_lines(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	for(int i = 1; i < 16; i++)
	{
		cg_set_line_cap(ctx, CG_LINE_CAP_ROUND);
		cg_set_line_width(ctx, i * 0.5);
		cg_set_source_rgb(ctx, 0, 0, 0);
		cg_move_to(ctx, 16 * i + 8, 16);
		cg_line_to(ctx, 16 * i - 8, 240);
		cg_stroke(ctx);
	}

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_multi_segment_caps(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	cg_move_to(ctx, 50.0, 75.0);
	cg_line_to(ctx, 200.0, 75.0);

	cg_move_to(ctx, 50.0, 125.0);
	cg_line_to(ctx, 200.0, 125.0);

	cg_move_to(ctx, 50.0, 175.0);
	cg_line_to(ctx, 200.0, 175.0);

	cg_set_line_width(ctx, 30.0);
	cg_set_line_cap(ctx, CG_LINE_CAP_ROUND);
	cg_stroke(ctx);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_rounded_rectangle(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	double x = 25.6,
		y = 25.6,
		width = 204.8,
		height = 204.8,
		aspect = 1.0,
		corner_radius = height / 10.0;
	double radius = corner_radius / aspect;
	double degrees = M_PI / 180.0;

	cg_new_path(ctx);
	cg_arc(ctx, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
	cg_arc(ctx, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
	cg_arc(ctx, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
	cg_arc(ctx, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
	cg_close_path(ctx);

	cg_set_source_rgb(ctx, 0.5, 0.5, 1);
	cg_fill_preserve(ctx);
	cg_set_source_rgba(ctx, 0.5, 0, 0, 0.5);
	cg_set_line_width(ctx, 10.0);
	cg_stroke(ctx);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_set_line_cap(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	cg_set_line_width(ctx, 30.0);
	cg_set_line_cap(ctx, CG_LINE_CAP_BUTT);
	cg_move_to(ctx, 64.0, 50.0);
	cg_line_to(ctx, 64.0, 200.0);
	cg_stroke(ctx);
	cg_set_line_cap(ctx, CG_LINE_CAP_ROUND);
	cg_move_to(ctx, 128.0, 50.0);
	cg_line_to(ctx, 128.0, 200.0);
	cg_stroke(ctx);
	cg_set_line_cap(ctx, CG_LINE_CAP_SQUARE);
	cg_move_to(ctx, 192.0, 50.0);
	cg_line_to(ctx, 192.0, 200.0);
	cg_stroke(ctx);

	cg_set_source_rgb(ctx, 1, 0.2, 0.2);
	cg_set_line_width(ctx, 2.56);
	cg_move_to(ctx, 64.0, 50.0);
	cg_line_to(ctx, 64.0, 200.0);
	cg_move_to(ctx, 128.0, 50.0);
	cg_line_to(ctx, 128.0, 200.0);
	cg_move_to(ctx, 192.0, 50.0);
	cg_line_to(ctx, 192.0, 200.0);
	cg_stroke(ctx);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_set_line_join(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	cg_set_line_width(ctx, 40.96);
	cg_move_to(ctx, 76.8, 84.48);
	cg_rel_line_to(ctx, 51.2, -51.2);
	cg_rel_line_to(ctx, 51.2, 51.2);
	cg_set_line_join(ctx, CG_LINE_JOIN_MITER);
	cg_stroke(ctx);

	cg_move_to(ctx, 76.8, 161.28);
	cg_rel_line_to(ctx, 51.2, -51.2);
	cg_rel_line_to(ctx, 51.2, 51.2);
	cg_set_line_join(ctx, CG_LINE_JOIN_BEVEL);
	cg_stroke(ctx);

	cg_move_to(ctx, 76.8, 238.08);
	cg_rel_line_to(ctx, 51.2, -51.2);
	cg_rel_line_to(ctx, 51.2, 51.2);
	cg_set_line_join(ctx, CG_LINE_JOIN_ROUND);
	cg_stroke(ctx);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_smile(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	double center_x = 256 * 0.5;
	double center_y = 256 * 0.5;
	double face_radius = 70;
	double eye_radius = 10;
	double mouth_radius = 50;
	double eye_offset_x = 25;
	double eye_offset_y = 20;
	double eye_x = center_x - eye_offset_x;
	double eye_y = center_y - eye_offset_y;

	cg_save(ctx);
	cg_arc(ctx, center_x, center_y, face_radius, 0, 2 * M_PI);
	cg_set_source_rgb(ctx, 1, 1, 0);
	cg_fill_preserve(ctx);
	cg_set_source_rgb(ctx, 0, 0, 0);
	cg_set_line_width(ctx, 5);
	cg_stroke(ctx);
	cg_restore(ctx);

	cg_save(ctx);
	cg_arc(ctx, eye_x, eye_y, eye_radius, 0, 2 * M_PI);
	cg_arc(ctx, center_x + eye_offset_x, eye_y, eye_radius, 0, 2 * M_PI);
	cg_fill(ctx);
	cg_restore(ctx);

	cg_save(ctx);
	cg_arc(ctx, center_x, center_y, mouth_radius, 0, M_PI);
	cg_set_line_width(ctx, 5);
	cg_stroke(ctx);
	cg_restore(ctx);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

static void test_texture_tiled(const char * filename)
{
	struct cg_surface_t * surface = cg_surface_create(256, 256);
	struct cg_ctx_t * ctx = cg_create(surface);

	struct cg_texture_t * tex = cg_texture_create(cg_surface_create_for_data(128, 128, (void *)cat_img_128_128));
	cg_texture_set_type(tex, CG_TEXTURE_TYPE_TILED);
	cg_translate(ctx, 128.0, 128.0);
	cg_rotate(ctx, -45 * M_PI / 180);
	cg_scale(ctx, 0.8, 0.8);
	cg_translate(ctx, -0.5 * tex->surface->width, -0.5 * tex->surface->height);
	cg_set_source_texture(ctx, tex);
	cg_paint(ctx);
	cg_texture_destroy(tex);

	cg_surface_write_to_png(surface, filename);
	cg_destroy(ctx);
	cg_surface_destroy(surface);
}

int main(int argc, char * argv[])
{
	test_arc("arc.png");
	test_arc_negative("arc_negative.png");
	test_clip("clip.png");
	test_clip_image("clip_image.png");
	test_curve_rectangle("curve_rectangle.png");
	test_curve_to("curve_to.png");
	test_dash("dash.png");
	test_fill_and_stroke("fill_and_stroke.png");
	test_fill_style("fill_style.png");
	test_gradient("gradient.png");
	test_image("image.png");
	test_lines("lines.png");
	test_multi_segment_caps("multi_segment_caps.png");
	test_rounded_rectangle("rounded_rectangle.png");
	test_set_line_cap("set_line_cap.png");
	test_set_line_join("set_line_join.png");
	test_smile("smile.png");
	test_texture_tiled("texture_tiled.png");
	return 0;
}
