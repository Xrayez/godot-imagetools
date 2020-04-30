#include "../thirdparty/exoquant/exoquant.h"
#include "core/io/image_loader.h"
#include "image_indexed.h"

const int ImageIndexed::MAX_PALETTE_SIZE = 256;

ImageIndexedMemLoadFunc ImageIndexed::_indexed_png_mem_loader_func = NULL;
SaveIndexedPNGFunc ImageIndexed::save_indexed_png_func = NULL;


Error ImageIndexed::create_indexed(int p_num_palette_entries) {

	ERR_FAIL_COND_V(empty(), ERR_UNCONFIGURED);

	// Indexed image width and height determined by this image
	const int width = get_width();
	const int height = get_height();

	const int num_pixels = width * height;

	index_data.resize(0);
	index_data.resize(num_pixels);
	{
		uint8_t *w = index_data.ptrw();
		zeromem(w, num_pixels);
	}

	const int ps = get_format_pixel_size(get_format());
	const int num_colors = int(CLAMP(p_num_palette_entries, 1, MAX_PALETTE_SIZE));

	palette_data.resize(0);
	palette_data.resize(num_colors * ps);
	{
		uint8_t *w = palette_data.ptrw();
		zeromem(w, num_colors * ps);
	}
	return OK;
}

Error ImageIndexed::create_indexed_from_data(const PackedByteArray &p_palette_data, const PackedByteArray &p_index_data) {

	ERR_FAIL_COND_V(empty(), ERR_UNCONFIGURED);
	ERR_FAIL_COND_V(p_index_data.size() == 0, ERR_CANT_CREATE);
	ERR_FAIL_COND_V(p_palette_data.size() == 0, ERR_CANT_CREATE);

	int ps = get_format_pixel_size(get_format());

	switch (ps) {
		case 3: break;
		case 4: break;
		default: {
			ERR_FAIL_V_MSG(ERR_CANT_CREATE, "Cannot create a palette, incompatible image format.");
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
		const uint8_t *r = p_index_data.ptr();

		for (int i = 0; i < index_size; ++i) {
			ERR_FAIL_COND_V_MSG(r[i] > palette_size - 1, ERR_INVALID_DATA, "Indices exceed (maximum) palette size.");
		}
	}
#endif
	palette_data = p_palette_data;
	index_data = p_index_data;

	return OK;
}

void ImageIndexed::set_pixel_indexed(int p_x, int p_y, int p_index) {
	uint8_t *w = index_data.ptrw();

#ifdef DEBUG_ENABLED
	ERR_FAIL_INDEX(p_x, get_width());
	ERR_FAIL_INDEX(p_y, get_height());
#endif
	uint32_t ofs = p_y * get_width() + p_x;
	w[ofs] = uint8_t(CLAMP(p_index, 0, MAX_PALETTE_SIZE - 1));
}

int ImageIndexed::get_pixel_indexed(int p_x, int p_y) const {
	const uint8_t *r = index_data.ptr();

#ifdef DEBUG_ENABLED
	ERR_FAIL_INDEX_V(p_x, get_width(), -1);
	ERR_FAIL_INDEX_V(p_y, get_height(), -1);
#endif
	uint32_t ofs = p_y * get_width() + p_x;
	int index = r[ofs];

	return index;
}

real_t ImageIndexed::generate_palette(int p_num_colors, DitherMode p_dither, bool p_with_alpha, bool p_high_quality) {

	ERR_FAIL_COND_V_MSG(empty(), -1.0, "Cannot generate a palette from an empty image.");
	ERR_FAIL_COND_V_MSG(get_format() != FORMAT_RGBA8, -1.0, "Cannot generate a palette, convert to FORMAT_RBGA8 first.");

	const int width = get_width();
	const int height = get_height();

	const int num_pixels = width * height;
	const int num_colors = int(CLAMP(p_num_colors, 1, MAX_PALETTE_SIZE));

	// Init
	PackedByteArray data = get_data();
	uint8_t *src = data.ptrw();

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

	uint8_t *pal_raw = palette_data.ptrw();

	exq_get_palette(pExq, pal_raw, num_colors);

	// Map indices to palette (doesn't overwrite original image)
	index_data.resize(0);
	index_data.resize(num_pixels);

	uint8_t *dest = index_data.ptrw();

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

void ImageIndexed::clear_palette() {

	palette_data.resize(0);
	index_data.resize(0);
}

Error ImageIndexed::apply_palette() {

	// Converts indexed data with associated palette to compatible format
	ERR_FAIL_COND_V_MSG(index_data.size() == 0, ERR_UNCONFIGURED, "No index data. Generate or create palette first.");
	ERR_FAIL_COND_V_MSG(palette_data.size() == 0, ERR_UNCONFIGURED, "No palette data. Generate or manually set palette first.");

	PackedByteArray dest_data;
	dest_data.resize(get_data().size());
	uint8_t *dest = dest_data.ptrw();

	const uint8_t *pal = palette_data.ptr();
	const uint8_t *ind = index_data.ptr();

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
			ERR_FAIL_V_MSG(ERR_UNAVAILABLE, "Cannot apply palette, unsupported format");
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

void ImageIndexed::set_palette(const PackedColorArray &p_palette) {

	ERR_FAIL_COND_MSG(index_data.size() == 0, "No index data. Generate or create palette first.");
	ERR_FAIL_COND_MSG(p_palette.size() != get_palette_size(), "Cannot set a palette with different size.");

	int ps = get_format_pixel_size(get_format());
	int num_colors = p_palette.size();

	const Color *r = p_palette.ptr();

	palette_data.resize(0);
	palette_data.resize(num_colors * ps);

	uint8_t *w = palette_data.ptrw();

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
			ERR_FAIL_MSG("Unsupported palette format");
		}
	}
}

PackedColorArray ImageIndexed::get_palette() const {

	ERR_FAIL_COND_V(!has_palette(), PackedColorArray());

	// Convert palette to bindable type
	int ps = get_format_pixel_size(get_format());
	int num_colors = palette_data.size() / ps;

	PackedColorArray palette;
	palette.resize(num_colors);

	Color *w = palette.ptrw();
	const uint8_t *r = palette_data.ptr();

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
			ERR_FAIL_V_MSG(PackedColorArray(), "Unsupported palette format");
		}
	}
	return palette;
}

void ImageIndexed::set_palette_color(int p_idx, const Color p_color) {

	ERR_FAIL_COND(!has_palette());

	int pixel_size = get_format_pixel_size(get_format());
	int ofs = p_idx * pixel_size;

	ERR_FAIL_INDEX(ofs, palette_data.size());

	uint8_t *ptr = palette_data.ptrw();

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
			ERR_FAIL_MSG("Unsupported palette format");
		}
	}
}

Color ImageIndexed::get_palette_color(int p_idx) const {

	ERR_FAIL_COND_V(!has_palette(), Color());

	int ps = get_format_pixel_size(get_format());
	int ofs = p_idx * ps;

	ERR_FAIL_INDEX_V(ofs, palette_data.size(), Color());

	const uint8_t *ptr = palette_data.ptr();

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
			ERR_FAIL_V_MSG(Color(), "Unsupported palette format");
		}
	}
	return Color();
}

PackedByteArray ImageIndexed::get_palette_data() const {

	return palette_data;
}

PackedByteArray ImageIndexed::get_index_data() const {

	return index_data;
}

Error ImageIndexed::load_indexed_png(const String &p_path) {

	Error err;
	PackedByteArray buffer;

	FileAccess *fr = FileAccess::open(p_path, FileAccess::READ, &err);
	if (!fr) {
		ERR_PRINT("Error opening file: " + p_path);
		return err;
	}

	int len = fr->get_len();
	buffer.resize(len);
	uint8_t *png = buffer.ptrw();
	fr->get_buffer(png, len);

	if (_indexed_png_mem_loader_func) {
		Ref<ImageIndexed> img = _indexed_png_mem_loader_func(png, len);
		copy_internals_from(img);
		create_indexed_from_data(img->get_palette_data(), img->get_index_data());
	}
	memdelete(fr);

	return err;
}

Error ImageIndexed::save_indexed_png(const String &p_path) const {

	if (save_indexed_png_func == NULL)
		return ERR_UNAVAILABLE;

	return save_indexed_png_func(p_path, Ref<ImageIndexed>((ImageIndexed *)this));
}

ImageIndexed::ImageIndexed() {
}

void ImageIndexed::_bind_methods() {

	ClassDB::bind_method(D_METHOD("create_indexed", "num_palette_entries"), &ImageIndexed::create_indexed, DEFVAL(MAX_PALETTE_SIZE));
	ClassDB::bind_method(D_METHOD("create_indexed_from_data", "palette_data", "index_data"), &ImageIndexed::create_indexed_from_data);

	ClassDB::bind_method(D_METHOD("set_pixel_indexed", "x", "y", "index"), &ImageIndexed::set_pixel_indexed);
	ClassDB::bind_method(D_METHOD("get_pixel_indexed", "x", "y"), &ImageIndexed::get_pixel_indexed);

	ClassDB::bind_method(D_METHOD("generate_palette", "num_colors", "dithering", "with_alpha", "high_quality"), &ImageIndexed::generate_palette, DEFVAL(MAX_PALETTE_SIZE), DEFVAL(DITHER_NONE), DEFVAL(true), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("clear_palette"), &ImageIndexed::clear_palette);
	ClassDB::bind_method(D_METHOD("apply_palette"), &ImageIndexed::apply_palette);

	ClassDB::bind_method(D_METHOD("has_palette"), &ImageIndexed::has_palette);
	ClassDB::bind_method(D_METHOD("get_palette_size"), &ImageIndexed::get_palette_size);

	ClassDB::bind_method(D_METHOD("set_palette", "palette"), &ImageIndexed::set_palette);
	ClassDB::bind_method(D_METHOD("get_palette"), &ImageIndexed::get_palette);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_COLOR_ARRAY, "palette", PROPERTY_HINT_NONE, "", 0), "set_palette", "get_palette");

	ClassDB::bind_method(D_METHOD("set_palette_color", "index", "color"), &ImageIndexed::set_palette_color);
	ClassDB::bind_method(D_METHOD("get_palette_color", "index"), &ImageIndexed::get_palette_color);

	ClassDB::bind_method(D_METHOD("load_indexed_png", "path"), &ImageIndexed::load_indexed_png);
	ClassDB::bind_method(D_METHOD("save_indexed_png", "path"), &ImageIndexed::save_indexed_png);

	BIND_CONSTANT(MAX_PALETTE_SIZE);

	BIND_ENUM_CONSTANT(DITHER_NONE);
	BIND_ENUM_CONSTANT(DITHER_ORDERED);
	BIND_ENUM_CONSTANT(DITHER_RANDOM);
}
