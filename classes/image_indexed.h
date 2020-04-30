#ifndef GODOT_IMAGE_INDEXED_H
#define GODOT_IMAGE_INDEXED_H

#include "core/image.h"

class ImageIndexed;

typedef Error (*SaveIndexedPNGFunc)(const String &p_path, const Ref<ImageIndexed> &p_img);
typedef Ref<ImageIndexed> (*ImageIndexedMemLoadFunc)(const uint8_t *p_png, int p_size);

class ImageIndexed : public Image {
	GDCLASS(ImageIndexed, Image);

private:
	PackedByteArray index_data;
	PackedByteArray palette_data;

protected:
	static void _bind_methods();

public:
	static const int MAX_PALETTE_SIZE;

	static ImageIndexedMemLoadFunc _indexed_png_mem_loader_func;
	static SaveIndexedPNGFunc save_indexed_png_func;

public:
	/**
	 * Indexed image and color palette interface.
	 *
	 * Can generate an optimal color palette for this image.
	 * The image can be loaded and saved as indexed.
	 * Loaded images will have a color palette (if present) and extended to
	 * compatible format, as there's no actual support for indexed images.
	 */
	enum DitherMode {
		DITHER_NONE,
		DITHER_ORDERED,
		DITHER_RANDOM,
	};
	Error create_indexed(int p_num_palette_entries = MAX_PALETTE_SIZE);
	Error create_indexed_from_data(const PackedByteArray &p_palette_data, const PackedByteArray &p_index_data);

	void set_pixel_indexed(int p_x, int p_y, int p_index);
	int get_pixel_indexed(int p_x, int p_y) const;

	real_t generate_palette(int p_num_colors = MAX_PALETTE_SIZE, DitherMode p_dither = DITHER_NONE, bool p_with_alpha = true, bool p_high_quality = false);
	void clear_palette();
	Error apply_palette();

	bool has_palette() const;
	int get_palette_size() const;

	void set_palette(const PackedColorArray &p_palette);
	PackedColorArray get_palette() const;

	void set_palette_color(int p_idx, const Color p_color);
	Color get_palette_color(int p_idx) const;

	PackedByteArray get_palette_data() const;
	PackedByteArray get_index_data() const;

	Error load_indexed_png(const String &p_path);
	Error save_indexed_png(const String &p_path) const;

	ImageIndexed();
};

VARIANT_ENUM_CAST(ImageIndexed::DitherMode)

#endif
