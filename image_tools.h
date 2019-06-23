#ifndef GODOT_IMAGE_TOOLS_H
#define GODOT_IMAGE_TOOLS_H

#include "core/image.h"

class ImageTools : public Object {
	GDCLASS(ImageTools, Object);

private:
	static ImageTools *singleton;

protected:
	static void _bind_methods();

public:
	static ImageTools *get_singleton() { return singleton; }

public:
	void replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color);

	ImageTools();
	~ImageTools();
};

#endif
