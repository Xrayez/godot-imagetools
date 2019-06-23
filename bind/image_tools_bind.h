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
	void replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color);

	_ImageTools();
};

#endif
