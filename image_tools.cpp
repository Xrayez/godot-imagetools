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

bool ImageTools::bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, Ref<Image> p_fill_image, KernelConnectivity p_kc) {

	if (!has_pixelv(p_image, p_at)) {
		return false;
	}

	p_image->lock();

	Color filling_color = p_image->get_pixelv(p_at);

	// A hack to match exact color in the image
	p_image->set_pixelv(p_at, p_fill_color);
	Color c = p_image->get_pixelv(p_at);
	// Restore pixel
	p_image->set_pixelv(p_at, filling_color);

	if (filling_color == c) {
		p_image->unlock();
		return false;
	}

	if (p_fill_image.is_valid()) {
		int width = p_image->get_width();
		int height = p_image->get_height();
		bool mipmaps = p_image->has_mipmaps();
		Image::Format format = p_image->get_format();

		p_fill_image->create(width, height, mipmaps, format);
		p_fill_image->lock();

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

				if (has_pixelv(p_fill_image, at)) {
					pixel = p_fill_image->get_pixelv(at);
					if (pixel.a > 0.0)
						continue; // already filled
				}
				if (has_pixelv(p_fill_image, at)) {
					pixel = p_image->get_pixelv(at);
				} else {
					continue;
				}
				if (pixel == filling_color) {
					p_fill_image->set_pixelv(at, p_fill_color);
					to_fill.push_back(at);
				}
			}
		}
	}
	p_fill_image->unlock();
	p_image->unlock();

	return true;
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
