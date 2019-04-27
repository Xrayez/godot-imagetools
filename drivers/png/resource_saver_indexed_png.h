#ifndef RESOURCE_SAVER_INDEXED_PNG_H
#define RESOURCE_SAVER_INDEXED_PNG_H

#include "image_indexed.h"
#include "core/io/resource_saver.h"

class ResourceSaverIndexedPNG : public ResourceFormatSaver {
	GDCLASS(ResourceSaverIndexedPNG, ResourceFormatSaver)
public:
	static Error save_image(const String &p_path, const Ref<ImageIndexed> &p_img);

	virtual Error save(const String &p_path, const RES &p_resource, uint32_t p_flags = 0);
	virtual bool recognize(const RES &p_resource) const;
	virtual void get_recognized_extensions(const RES &p_resource, List<String> *p_extensions) const;

	ResourceSaverIndexedPNG();
};

#endif // RESOURCE_SAVER_INDEXED_PNG_H
