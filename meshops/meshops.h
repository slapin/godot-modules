#ifndef MESHOPS_H
#define MESHOPS_H
#include <core/reference.h>
#include <scene/resources/mesh.h>
#include <scene/resources/material.h>

class MeshItemList: public Reference {
	GDCLASS(ItemList, Reference)
protected:
	static void _bind_methods();
private:
	struct mesh_data {
		Ref<Mesh> mesh;
		Array mesh_surfaces;
		Vector<Ref<Material> > materials;
		StringName item_name;
	};
	HashMap<String, struct mesh_data> item_list;
public:
	void add_item(const StringName &item_name, Ref<Mesh> mesh);
	void remove_item(const StringName &item_name);
	const Array &get_item_arrays(const StringName &item_name) const;
	Ref<Mesh> get_item_mesh(const StringName &item_name) const;
}
class MeshOps: public Reference {
	GDCLASS(MeshOps, Reference)
protected:
	static void _bind_methods();
public:
	static MeshOps *get_singleton();
	Array merge_meshes(const Array &surfaces, const Array &xforms) const;
};

class GenCitySet: public Resource {
	GDCLASS(GenCitySet, Resource)
protected:
	static void _bind_methods();
	Ref<Resource> guildhall_building_set;
	Ref<Resource> court_building_set;
	Array building_sets;
	float center_radius;
	float radius;
	int min_buildings;
	int max_buildings;
public:
	void set_guildhall_building_set(const Ref<Resource> set);
	Ref<Resource> get_guildhall_building_set() const;
	void set_court_building_set(const Ref<Resource> set);
	Ref<Resource> get_court_building_set() const;
	void set_building_sets(const Array &sets);
	const Array &get_building_sets() const;
	void set_center_radius(float radius);
	float get_center_radius() const;
	void set_radius(float radius);
	float get_radius() const;
	void set_min_buildings(int minb);
	void set_max_buildings(int maxb);
};
#endif

