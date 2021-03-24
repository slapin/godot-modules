#ifndef TOWN_H
#define TOWN_H
#include <core/reference.h>
#include <scene/resources/mesh.h>
#include <scene/resources/material.h>

// #include "city_grid.h"

class MeshItemList: public Reference {
	GDCLASS(MeshItemList, Reference)
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
};

class GenBuildingSet;
class GenRoadSet;

class GenCitySet: public Resource {
	GDCLASS(GenCitySet, Resource)
protected:
	static void _bind_methods();
	Ref<GenBuildingSet> guildhall_building_set;
	Ref<GenBuildingSet> court_building_set;
	Ref<GenRoadSet> road_set;
	Array building_sets;
	float center_radius;
	float radius;
	int min_buildings;
	int max_buildings;
public:
	void set_guildhall_building_set(const Ref<GenBuildingSet> set);
	Ref<GenBuildingSet> get_guildhall_building_set() const;
	void set_court_building_set(const Ref<GenBuildingSet> set);
	Ref<GenBuildingSet> get_court_building_set() const;
	void set_road_set(const Ref<GenRoadSet> set);
	Ref<GenRoadSet> get_road_set() const;
	void set_building_sets(const Array &sets);
	const Array &get_building_sets() const;
	void set_center_radius(float radius);
	float get_center_radius() const;
	void set_radius(float radius);
	float get_radius() const;
	void set_min_buildings(int minb);
	int get_min_buildings() const;
	void set_max_buildings(int maxb);
	int get_max_buildings() const;
	void _set_data(const Dictionary &data);
	Dictionary _get_data() const;
	Dictionary get_items() const;
};
class GenExteriorSet: public Resource {
	GDCLASS(GenExteriorSet, Resource)
protected:
	HashMap<String, Ref<ArrayMesh> > _data;
	HashMap<String, String > xmap;
	bool _set(const StringName &name, const Variant &property);
	bool _get(const StringName &name, Variant &property) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
public:
	GenExteriorSet();
};
class GenInteriorSet: public Resource {
	GDCLASS(GenInteriorSet, Resource)
protected:
	HashMap<String, Ref<ArrayMesh> > _data;
	HashMap<String, String > xmap;
	bool _set(const StringName &name, const Variant &property);
	bool _get(const StringName &name, Variant &property) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
public:
	GenInteriorSet();
};
class GenFurnitureSet: public Resource {
	GDCLASS(GenFurnitureSet, Resource)
};

class GenBuildingSet: public Resource {
	GDCLASS(GenBuildingSet, Resource)
protected:
	HashMap<String, int> _datai;
	HashMap<String, String> _datas;
	HashMap<String, float> _dataf;
	HashMap<String, Ref<Resource> > _datar;
	bool _set(const StringName &name, const Variant &property);
	bool _get(const StringName &name, Variant &property) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
public:
	GenBuildingSet();
};
class GenRoadSet: public Resource {
	GDCLASS(GenRoadSet, Resource)
protected:
	HashMap<String, Ref<ArrayMesh> > _data;
	float segment_length,
	      segment_width,
	      short_segment_length,
	      turn_angle,
	      turn_angle2;
	bool _set(const StringName &name, const Variant &property);
	bool _get(const StringName &name, Variant &property) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
public:
	GenRoadSet();
};
#endif

