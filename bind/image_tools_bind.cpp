#include "image_tools_bind.h"
#include "image_tools.h"

_ImageTools *_ImageTools::singleton = NULL;

void _ImageTools::replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color) {

	ImageTools::replace_color(p_image, p_color, p_with_color);
}

Ref<Image> _ImageTools::bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, bool p_fill_image, KernelConnectivity p_kc) {

	return ImageTools::bucket_fill(p_image, p_at, p_fill_color, p_fill_image, ImageTools::KernelConnectivity(p_kc));
}

bool _ImageTools::has_pixel(Ref<Image> p_image, int x, int y) {

	return ImageTools::has_pixel(p_image, x, y);
}

bool _ImageTools::has_pixelv(Ref<Image> p_image, const Vector2 &p_pos) {

	return ImageTools::has_pixelv(p_image, p_pos);
}

Variant _ImageTools::get_pixel_or_null(Ref<Image> p_image, int x, int y) {
	Variant ret;
	Color pixel;
	if(ImageTools::get_pixel_or_null(p_image, x, y, &pixel)) {
		ret = pixel;
	}
	return ret;
}

Variant _ImageTools::get_pixelv_or_null(Ref<Image> p_image, const Vector2 &p_pos) {
	
	return _ImageTools::get_pixel_or_null(p_image, p_pos.x, p_pos.y);
}

void _ImageTools::_bind_methods() {

	// Image methods
	ClassDB::bind_method(D_METHOD("replace_color", "image", "color", "with_color"), &_ImageTools::replace_color);
	ClassDB::bind_method(D_METHOD("bucket_fill", "image", "at", "fill_color", "fill_image", "connectivity"), &_ImageTools::bucket_fill, DEFVAL(true), DEFVAL(KERNEL_FOUR_WAY));
	
	// Pixel methods
	ClassDB::bind_method(D_METHOD("get_pixel_or_null", "image", "x", "y"), &_ImageTools::get_pixel_or_null);
	ClassDB::bind_method(D_METHOD("get_pixelv_or_null", "image", "pos"), &_ImageTools::get_pixelv_or_null);
	ClassDB::bind_method(D_METHOD("has_pixel", "image", "x", "y"), &_ImageTools::has_pixel);
	ClassDB::bind_method(D_METHOD("has_pixelv", "image", "pos"), &_ImageTools::has_pixelv);

	BIND_ENUM_CONSTANT(KERNEL_FOUR_WAY);
	BIND_ENUM_CONSTANT(KERNEL_EIGHT_WAY);
}

_ImageTools::_ImageTools() {
	singleton = this;
}
