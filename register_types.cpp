#include "register_types.h"

#include "classes/image_indexed.h"

void register_imagetools_types() {

	ClassDB::register_class<ImageIndexed>();
}

void unregister_imagetools_types() {

	// nothing to do here
}
