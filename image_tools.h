#ifndef GODOT_IMAGE_TOOLS_H
#define GODOT_IMAGE_TOOLS_H

#include "core/image.h"

class ImageTools {
	
public:
	enum KernelConnectivity {
		KERNEL_FOUR_WAY,
		KERNEL_EIGHT_WAY,
	};

public:
	static void replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color);
	static bool bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, Ref<Image> p_fill_image, KernelConnectivity p_kc = KERNEL_FOUR_WAY);
	
public:
	static bool has_pixel(Ref<Image> p_image, int x, int y);
	static bool has_pixelv(Ref<Image> p_image, const Vector2 &p_pos);
};

#endif
