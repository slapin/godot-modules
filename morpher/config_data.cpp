#include "config_data.h"
#include <core/io/json.h>
#include <core/os/file_access.h>
#include <cassert>
ConfigData::ConfigData() {
	FileAccess *fd = FileAccess::open("res://characters/config.json", FileAccess::READ);
	assert(fd);
	String confdata = fd->get_as_utf8_string();
	fd->close();
	String err;
	int err_line;
	Variant adata;
	Error e = JSON::parse(confdata, adata, err, err_line);
	if (e != OK)
		printf("json parse error: %ls at line %d\n", err.c_str(), err_line);
	assert(e == OK);
	config = adata;
}
ConfigData *ConfigData::get_singleton() {
	static ConfigData data;
	return &data;
}
AccessoryData::AccessoryData() {
	const String &accessory_path =
			ConfigData::get_singleton()->get()["accessory_path"];
	FileAccess *fd = FileAccess::open(accessory_path, FileAccess::READ);
	assert(fd);
	String confdata = fd->get_as_utf8_string();
	fd->close();
	String err;
	int err_line;
	Variant adata;
	Error e = JSON::parse(confdata, adata, err, err_line);
	if (e != OK)
		printf("json parse error: %ls at line %d\n", err.c_str(), err_line);
	assert(e == OK);
	accessory = adata;
}
AccessoryData *AccessoryData::get_singleton() {
	static AccessoryData data;
	return &data;
}
Ref<ArrayMesh> AccessoryData::get_mesh(const Dictionary &entry) const {
	const Array materials = entry["materials"];
	const String &mesh_path = entry["path"];
	int i;
	Error err = OK;

	Ref<ArrayMesh> mesh_orig = ResourceLoader::load(mesh_path, "", &err);
	Ref<ArrayMesh> mesh = mesh_orig->duplicate();
	mesh->set_meta("orig_path", mesh_orig->get_path());
	if (err != OK) {
		printf("Could not read resource %ls\n", mesh_path.c_str());
		return NULL;
	}
	for (i = 0; i < mesh->get_surface_count(); i++) {
		const Dictionary &mat_data = materials[i];
		const String &mat_path = mat_data["path"];
		Ref<Material> mat = ResourceLoader::load(mat_path, "", &err);
		mesh->surface_set_material(i, mat);
	}
	return mesh;
}
