#include "register_types.h"

#include "drivers/png/image_loader_indexed_png.h"
#include "drivers/png/resource_saver_indexed_png.h"

#include "image_indexed.h"

static ImageLoaderIndexedPNG *image_loader_indexed_png;
static Ref<ResourceSaverIndexedPNG> resource_saver_indexed_png;

void register_imagetools_types() {

	ClassDB::register_class<ImageIndexed>();

	image_loader_indexed_png = memnew(ImageLoaderIndexedPNG);
	ImageLoader::add_image_format_loader(image_loader_indexed_png);

	resource_saver_indexed_png.instance();
	ResourceSaver::add_resource_format_saver(resource_saver_indexed_png);
}

void unregister_imagetools_types() {

	if (image_loader_indexed_png)
		memdelete(image_loader_indexed_png);

	ResourceSaver::remove_resource_format_saver(resource_saver_indexed_png);
	resource_saver_indexed_png.unref();
}
