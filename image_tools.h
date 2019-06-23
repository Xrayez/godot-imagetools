#ifndef GODOT_IMAGE_TOOLS_H
#define GODOT_IMAGE_TOOLS_H

#include "core/image.h"

class ImageTools {

public:
	static void replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color);
};

#endif
