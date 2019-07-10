#ifndef GODOT_IMAGE_TOOLS_BIND_H
#define GODOT_IMAGE_TOOLS_BIND_H

#include "core/image.h"

class _ImageTools : public Object {
	GDCLASS(_ImageTools, Object);

private:
	static _ImageTools *singleton;

protected:
	static void _bind_methods();

public:
	static _ImageTools *get_singleton() { return singleton; }

public:
	enum KernelConnectivity {
		KERNEL_FOUR_WAY,
		KERNEL_EIGHT_WAY,
	};

public:
	void replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color);
	Ref<Image> bucket_fill(Ref<Image> p_image, const Point2 &p_at, const Color &p_fill_color, bool p_fill_image = true, KernelConnectivity p_kc = KERNEL_FOUR_WAY);

	_ImageTools();
};

VARIANT_ENUM_CAST(_ImageTools::KernelConnectivity);

#endif
