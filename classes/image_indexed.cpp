#include "../thirdparty/exoquant/exoquant.h"
#include "image_indexed.h"

const int ImageIndexed::MAX_PALETTE_SIZE = 256;

ImageIndexedMemLoadFunc ImageIndexed::_indexed_png_mem_loader_func = NULL;
SaveIndexedPNGFunc ImageIndexed::save_indexed_png_func = NULL;


real_t ImageIndexed::generate_palette(int p_num_colors, DitherMode p_dither, bool p_with_alpha, bool p_high_quality) {

	ERR_EXPLAIN("Cannot generate a palette from an empty image.");
	ERR_FAIL_COND_V(empty(), -1.0);

	ERR_EXPLAIN("Cannot generate a palette, convert to FORMAT_RBGA8 first.");
	ERR_FAIL_COND_V(get_format() != FORMAT_RGBA8, -1.0);

	const int width = get_width();
	const int height = get_height();

	const int num_pixels = width * height;
	const int num_colors = int(CLAMP(p_num_colors, 1, MAX_PALETTE_SIZE));

	// Init
	PoolVector<uint8_t> data = get_data();
	PoolVector<uint8_t>::Write w_src = data.write();
	uint8_t *src = w_src.ptr();

	exq_data *pExq = exq_init();
	if (!p_with_alpha) {
		exq_no_transparency(pExq);
	}
	exq_feed(pExq, src, num_pixels);

	// Quantize
	exq_quantize_ex(pExq, num_colors, (int)p_high_quality);

	// Extract palette
	palette_data.resize(0);
	palette_data.resize(num_colors * 4);

	PoolVector<uint8_t>::Write w_pal_raw = palette_data.write();
	uint8_t *pal_raw = w_pal_raw.ptr();

	exq_get_palette(pExq, pal_raw, num_colors);

	// Map indices to palette (doesn't overwrite original image)
	index_data.resize(0);
	index_data.resize(num_pixels);

	PoolVector<uint8_t>::Write w_dest = index_data.write();
	uint8_t *dest = w_dest.ptr();

	switch (p_dither) {
		case DITHER_NONE: {
			exq_map_image(pExq, num_pixels, src, dest);
		} break;
		case DITHER_ORDERED: {
			exq_map_image_ordered(pExq, width, height, src, dest);
		} break;
		case DITHER_RANDOM: {
			exq_map_image_random(pExq, num_pixels, src, dest);
		} break;
	}
	// Determine quantization quality
	real_t mean_error = exq_get_mean_error(pExq);

	// Cleanup
	exq_free(pExq);

	return mean_error;
}

Error ImageIndexed::create_palette(const PoolVector<uint8_t> &p_palette_data, const PoolVector<uint8_t> &p_index_data) {

	ERR_FAIL_COND_V(empty(), ERR_UNCONFIGURED);
	ERR_FAIL_COND_V(p_index_data.size() == 0, ERR_CANT_CREATE);
	ERR_FAIL_COND_V(p_palette_data.size() == 0, ERR_CANT_CREATE);

	int ps = get_format_pixel_size(get_format());

	switch (ps) {
		case 3: break;
		case 4: break;
		default: {
			ERR_EXPLAIN("Cannot create a palette, incompatible image format.");
			ERR_FAIL_V(ERR_CANT_CREATE);
		}
	}
	ERR_FAIL_COND_V(palette_data.size() % ps != 0, ERR_INVALID_DATA);

	int palette_size = p_palette_data.size() / ps;
	ERR_FAIL_COND_V(palette_size > MAX_PALETTE_SIZE, ERR_CANT_CREATE);

	int index_size = p_index_data.size();
	int num_pixels = get_width() * get_height();
	ERR_FAIL_COND_V(index_size != num_pixels, ERR_CANT_CREATE);

#ifdef DEBUG_ENABLED
	// Ensure all indices point to valid palette entries
	{
		PoolVector<uint8_t>::Read ind = p_index_data.read();

		for (int i = 0; i < index_size; ++i) {
			ERR_EXPLAIN("Indices exceed (maximum) palette size.");
			ERR_FAIL_COND_V(ind[i] > palette_size - 1, ERR_INVALID_DATA);
		}
	}
#endif
	palette_data = p_palette_data;
	index_data = p_index_data;

	return OK;
}

void ImageIndexed::clear_palette() {

	palette_data.resize(0);
	index_data.resize(0);
}

Error ImageIndexed::apply_palette() {

	// Converts indexed data with associated palette to compatible format

	ERR_EXPLAIN("No index data. Generate or create palette first.");
	ERR_FAIL_COND_V(index_data.size() == 0, ERR_UNCONFIGURED);

	ERR_EXPLAIN("No palette data. Generate or manually set palette first.");
	ERR_FAIL_COND_V(palette_data.size() == 0, ERR_UNCONFIGURED);

	PoolVector<uint8_t> dest_data;
	dest_data.resize(get_data().size());
	PoolVector<uint8_t>::Write w_dest = dest_data.write();
	uint8_t *dest = w_dest.ptr();

	PoolVector<uint8_t>::Read pal = palette_data.read();
	PoolVector<uint8_t>::Read ind = index_data.read();

	int ps = get_format_pixel_size(get_format());
	int num_pixels = get_width() * get_height();

	switch (ps) {
		case 3: {
			for (int i = 0; i < num_pixels; i++) {
				dest[0] = pal[ind[i] * ps + 0];
				dest[1] = pal[ind[i] * ps + 1];
				dest[2] = pal[ind[i] * ps + 2];

				dest += ps;
			}
		} break;
		case 4: {
			for (int i = 0; i < num_pixels; i++) {
				dest[0] = pal[ind[i] * ps + 0];
				dest[1] = pal[ind[i] * ps + 1];
				dest[2] = pal[ind[i] * ps + 2];
				dest[3] = pal[ind[i] * ps + 3];

				dest += ps;
			}
		} break;
		default: {
			ERR_EXPLAIN("Cannot apply palette, unsupported format");
			ERR_FAIL_V(ERR_UNAVAILABLE);
		}
	}
	bool used_mipmaps = has_mipmaps();

	create(get_width(), get_height(), has_mipmaps(), get_format(), dest_data);

	if (used_mipmaps) {
		generate_mipmaps();
	}
	return OK;
}

bool ImageIndexed::has_palette() const {

	return palette_data.size() > 0;
}

int ImageIndexed::get_palette_size() const {

	int pixel_size = get_format_pixel_size(get_format());

	ERR_FAIL_COND_V(palette_data.size() % pixel_size != 0, -1);

	return palette_data.size() / pixel_size;
}

void ImageIndexed::set_palette(const PoolColorArray &p_palette) {

	ERR_EXPLAIN("No index data. Generate or create palette first.");
	ERR_FAIL_COND(index_data.size() == 0);

	ERR_EXPLAIN("Cannot set a palette with different size.")
	ERR_FAIL_COND(p_palette.size() != get_palette_size());

	int ps = get_format_pixel_size(get_format());
	int num_colors = p_palette.size();

	PoolColorArray::Read r = p_palette.read();

	palette_data.resize(0);
	palette_data.resize(num_colors * ps);

	PoolVector<uint8_t>::Write w_pal = palette_data.write();
	uint8_t *w = w_pal.ptr();

	switch (ps) {
		case 3: {
			for (int i = 0; i < num_colors; i++) {
				w[0] = r[i].r * 255.0f;
				w[1] = r[i].g * 255.0f;
				w[2] = r[i].b * 255.0f;

				w += ps;
			}
		} break;
		case 4: {
			for (int i = 0; i < num_colors; i++) {
				w[0] = r[i].r * 255.0f;
				w[1] = r[i].g * 255.0f;
				w[2] = r[i].b * 255.0f;
				w[3] = r[i].a * 255.0f;

				w += ps;
			}
		} break;
		default: {
			ERR_EXPLAIN("Unsupported palette format");
			ERR_FAIL();
		}
	}
}

PoolColorArray ImageIndexed::get_palette() const {

	ERR_FAIL_COND_V(!has_palette(), PoolColorArray());

	// Convert palette to bindable type
	int ps = get_format_pixel_size(get_format());
	int num_colors = palette_data.size() / ps;

	PoolColorArray palette;
	palette.resize(num_colors);

	PoolColorArray::Write w = palette.write();

	PoolVector<uint8_t>::Read r_pal = palette_data.read();
	const uint8_t *r = r_pal.ptr();

	switch (ps) {
		case 3: {
			for (int i = 0; i < num_colors; i++) {
				float rc = r[i * ps + 0] / 255.0f;
				float gc = r[i * ps + 1] / 255.0f;
				float bc = r[i * ps + 2] / 255.0f;

				w[i] = Color(rc, gc, bc);
			}
		} break;
		case 4: {
			for (int i = 0; i < num_colors; i++) {
				float rc = r[i * ps + 0] / 255.0f;
				float gc = r[i * ps + 1] / 255.0f;
				float bc = r[i * ps + 2] / 255.0f;
				float ac = r[i * ps + 3] / 255.0f;

				w[i] = Color(rc, gc, bc, ac);
			}
		} break;
		default: {
			ERR_EXPLAIN("Unsupported palette format");
			ERR_FAIL_V(PoolColorArray());
		}
	}
	return palette;
}

void ImageIndexed::set_palette_color(int p_idx, const Color p_color) {

	ERR_FAIL_COND(!has_palette());

	int pixel_size = get_format_pixel_size(get_format());
	int ofs = p_idx * pixel_size;

	ERR_FAIL_INDEX(ofs, palette_data.size());

	PoolVector<uint8_t>::Write write = palette_data.write();
	uint8_t *ptr = write.ptr();

	switch (pixel_size) {
		case 3: {
			ptr[ofs + 0] = uint8_t(CLAMP(p_color.r * 255.0, 0, 255));
			ptr[ofs + 1] = uint8_t(CLAMP(p_color.g * 255.0, 0, 255));
			ptr[ofs + 2] = uint8_t(CLAMP(p_color.b * 255.0, 0, 255));
		} break;
		case 4: {
			ptr[ofs + 0] = uint8_t(CLAMP(p_color.r * 255.0, 0, 255));
			ptr[ofs + 1] = uint8_t(CLAMP(p_color.g * 255.0, 0, 255));
			ptr[ofs + 2] = uint8_t(CLAMP(p_color.b * 255.0, 0, 255));
			ptr[ofs + 3] = uint8_t(CLAMP(p_color.a * 255.0, 0, 255));
		} break;
		default: {
			ERR_EXPLAIN("Unsupported palette format");
			ERR_FAIL();
		}
	}
}

Color ImageIndexed::get_palette_color(int p_idx) const {

	ERR_FAIL_COND_V(!has_palette(), Color());

	int ps = get_format_pixel_size(get_format());
	int ofs = p_idx * ps;

	ERR_FAIL_INDEX_V(ofs, palette_data.size(), Color());

	PoolVector<uint8_t>::Read read = palette_data.read();
	const uint8_t *ptr = read.ptr();

	switch (ps) {
		case 3: {
			float r = ptr[ofs + 0] / 255.0;
			float g = ptr[ofs + 1] / 255.0;
			float b = ptr[ofs + 2] / 255.0;

			return Color(r, g, b);
		} break;
		case 4: {
			float r = ptr[ofs + 0] / 255.0;
			float g = ptr[ofs + 1] / 255.0;
			float b = ptr[ofs + 2] / 255.0;
			float a = ptr[ofs + 3] / 255.0;

			return Color(r, g, b, a);
		} break;
		default: {
			ERR_EXPLAIN("Unsupported palette format");
			ERR_FAIL_V(Color());
		}
	}
	return Color();
}

PoolVector<uint8_t> ImageIndexed::get_palette_data() const {

	return palette_data;
}

PoolVector<uint8_t> ImageIndexed::get_index_data() const {

	return index_data;
}

// Error ImageIndexed::load_png(const String &p_path) {
// 	return ImageLoader::load_image(p_path, this);
// }

Error ImageIndexed::save_indexed_png(const String &p_path) const {

	if (save_indexed_png_func == NULL)
		return ERR_UNAVAILABLE;

	return save_indexed_png_func(p_path, Ref<ImageIndexed>((ImageIndexed *)this));
}

ImageIndexed::ImageIndexed() {
}

void ImageIndexed::_bind_methods() {

	ClassDB::bind_method(D_METHOD("generate_palette", "num_colors", "dithering", "with_alpha", "high_quality"), &ImageIndexed::generate_palette, DEFVAL(MAX_PALETTE_SIZE), DEFVAL(DITHER_NONE), DEFVAL(true), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("clear_palette"), &ImageIndexed::clear_palette);
	ClassDB::bind_method(D_METHOD("apply_palette"), &ImageIndexed::apply_palette);

	ClassDB::bind_method(D_METHOD("has_palette"), &ImageIndexed::has_palette);
	ClassDB::bind_method(D_METHOD("get_palette_size"), &ImageIndexed::get_palette_size);

	ClassDB::bind_method(D_METHOD("set_palette", "palette"), &ImageIndexed::set_palette);
	ClassDB::bind_method(D_METHOD("get_palette"), &ImageIndexed::get_palette);
	ADD_PROPERTY(PropertyInfo(Variant::POOL_COLOR_ARRAY, "palette", PROPERTY_HINT_NONE, "", 0), "set_palette", "get_palette");

	ClassDB::bind_method(D_METHOD("set_palette_color", "index", "color"), &ImageIndexed::set_palette_color);
	ClassDB::bind_method(D_METHOD("get_palette_color", "index"), &ImageIndexed::get_palette_color);

	ClassDB::bind_method(D_METHOD("save_indexed_png", "path"), &ImageIndexed::save_indexed_png);

	BIND_CONSTANT(MAX_PALETTE_SIZE);

	BIND_ENUM_CONSTANT(DITHER_NONE);
	BIND_ENUM_CONSTANT(DITHER_ORDERED);
	BIND_ENUM_CONSTANT(DITHER_RANDOM);
}
