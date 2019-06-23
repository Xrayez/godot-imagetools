#include "image_tools_bind.h"
#include "image_tools.h"

_ImageTools* _ImageTools::singleton = NULL;

void _ImageTools::replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color) {
    
    ImageTools::replace_color(p_image, p_color, p_with_color);
}

void _ImageTools::_bind_methods() {
    
    ClassDB::bind_method(D_METHOD("replace_color", "image", "color", "with_color"), &_ImageTools::replace_color);    
}

_ImageTools::_ImageTools() {
    singleton = this;
}
