#include "image_tools.h"

void ImageTools::replace_color(Ref<Image> p_image, const Color &p_color, const Color &p_with_color) {
    
    if (p_color == p_with_color) {
        return;
    }
    
    p_image->lock();
    
    for (int y = 0; y < p_image->get_height(); ++y) {
        for (int x = 0; x < p_image->get_width(); ++x) {
            
            if(p_image->get_pixel(x, y) == p_color) {
                p_image->set_pixel(x, y, p_with_color);
            }
        }
    }
    p_image->unlock();
}
