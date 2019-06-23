#include "register_types.h"

#include "core/engine.h"

#include "drivers/png/image_loader_indexed_png.h"
#include "drivers/png/resource_saver_indexed_png.h"

#include "bind/image_tools_bind.h"
#include "image_indexed.h"
#include "image_tools.h"

static _ImageTools *_image_tools = NULL;

static ImageLoaderIndexedPNG *image_loader_indexed_png;
static Ref<ResourceSaverIndexedPNG> resource_saver_indexed_png;

void register_imagetools_types() {

	// Singleton
	_image_tools = memnew(_ImageTools);

	ClassDB::register_class<_ImageTools>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("ImageTools", _ImageTools::get_singleton()));

	// Indexed image
	ClassDB::register_class<ImageIndexed>();

	image_loader_indexed_png = memnew(ImageLoaderIndexedPNG);
	ImageLoader::add_image_format_loader(image_loader_indexed_png);

	resource_saver_indexed_png.instance();
	ResourceSaver::add_resource_format_saver(resource_saver_indexed_png);
}

void unregister_imagetools_types() {

	memdelete(_image_tools);

	if (image_loader_indexed_png)
		memdelete(image_loader_indexed_png);

	ResourceSaver::remove_resource_format_saver(resource_saver_indexed_png);
	resource_saver_indexed_png.unref();
}
