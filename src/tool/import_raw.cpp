#include "pch.h"
#include "tool.h"

void import_raw(asset_file_builder* afb, const char* path, const char* name, u32 code) {
	file_buf f;

	if (!load_file(&f, path)) {
		debug("import_raw: failed to load - %s", path);
		return;
	}

	asset* a = new_asset(afb, code, name);

	if (!a) {
		debug("import_raw: failed to allocate asset - %s", path);
		return;
	}

	u32 total_size = sizeof(raw_header) + f.size;

	if (!a->data.set_size(total_size)) {
		debug("import_raw: failed to allocate asset data - %s", path);
		return;
	}

	raw_header*	h = (raw_header*)a->data.begin();
	u8*			d = a->data.begin() + sizeof(raw_header);

	h->size = f.size;

	memcpy(d, f.data, f.size);
}
