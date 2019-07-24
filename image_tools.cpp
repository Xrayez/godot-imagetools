#include "image_tools.h"

void ImageTools::replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color) {

	if (p_color == p_with_color) {
		return;
	}

	p_image->lock();

	for (int y = 0; y < p_image->get_height(); ++y) {
		for (int x = 0; x < p_image->get_width(); ++x) {

			if (p_image->get_pixel(x, y) == p_color) {
				p_image->set_pixel(x, y, p_with_color);
			}
		}
	}
	p_image->unlock();
}

Ref<Image> ImageTools::bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, bool p_fill_image, KernelConnectivity p_kc) {

	// Based on flood-fill algorithm
	// Runs up to x35 faster compared to GDScript implementation

	if (!has_pixelv(p_image, p_at)) {
		return Ref<Image>();
	}
	p_image->lock();

	Color filling_color = p_image->get_pixelv(p_at);

	int width = p_image->get_width();
	int height = p_image->get_height();
	bool mipmaps = p_image->has_mipmaps();
	Image::Format format = p_image->get_format();

	Ref<Image> fill_image = memnew(Image);
	fill_image->create(width, height, mipmaps, format);
	fill_image->lock();

	Vector2 at;
	Vector2 pos = p_at;
	Color pixel;

	Vector<Vector2> kernel;
	switch (p_kc) {
		case KERNEL_FOUR_WAY: {
			kernel.push_back(Vector2(1, 0));
			kernel.push_back(Vector2(0, -1));
			kernel.push_back(Vector2(-1, 0));
			kernel.push_back(Vector2(0, 1));
		} break;

		case KERNEL_EIGHT_WAY: {
			kernel.push_back(Vector2(1, 0));
			kernel.push_back(Vector2(1, -1));
			kernel.push_back(Vector2(0, -1));
			kernel.push_back(Vector2(-1, -1));
			kernel.push_back(Vector2(-1, 0));
			kernel.push_back(Vector2(-1, 1));
			kernel.push_back(Vector2(0, 1));
			kernel.push_back(Vector2(1, 1));
		} break;
	}

	List<Vector2> to_fill;
	to_fill.push_back(pos);

	while (!to_fill.empty()) {
		pos = to_fill.front()->get();
		to_fill.pop_front();

		for (int i = 0; i < kernel.size(); ++i) {

			const Vector2 &dir = kernel[i];
			at = pos + dir;

			if (has_pixelv(fill_image, at)) {
				pixel = fill_image->get_pixelv(at);
				if (pixel.a > 0.0)
					continue; // already filled
			}
			if (has_pixelv(fill_image, at)) {
				pixel = p_image->get_pixelv(at);
			} else {
				continue;
			}
			if (pixel == filling_color) {
				fill_image->set_pixelv(at, p_fill_color);
				to_fill.push_back(at);
			}
		}
	}
	if (p_fill_image) {
		// Fill the actual image (no undo)
		// else just return filled area as a new image
		Rect2 fill_rect(0, 0, width, height);
		p_image->blend_rect(fill_image, fill_rect, Point2());
	}

	fill_image->unlock();
	p_image->unlock();

	return fill_image;
}

bool ImageTools::has_pixel(Ref<Image> p_image, int x, int y) {

	const int width = p_image->get_width();
	const int height = p_image->get_height();

	if (x >= 0 && x < width && y >= 0 && y < height) {
		return true;
	}
	return false;
}

bool ImageTools::has_pixelv(Ref<Image> p_image, const Vector2 &p_pos) {

	return has_pixel(p_image, p_pos.x, p_pos.y);
}
