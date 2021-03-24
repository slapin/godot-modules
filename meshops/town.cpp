#include "town.h"

void MeshItemList::add_item(const StringName &item_name, Ref<Mesh> mesh)
{
	struct mesh_data item;
	int surface_count = 0, i;
	item.item_name = item_name;
	item.mesh = mesh;
	Array surfaces;
	Vector<Ref<Material> > materials;
	surface_count = mesh->get_surface_count();
	surfaces.resize(surface_count);
	materials.resize(surface_count);
	for (i = 0; i < surface_count; i++) {
		surfaces[i] = mesh->surface_get_arrays(i);
		Ref<Material> mat = mesh->surface_get_material(i);
		materials.write[i] = mat;
	}

	item.mesh_surfaces = surfaces;
	item.materials = materials;
	item_list[item_name] = item;
}
void MeshItemList::remove_item(const StringName &item_name)
{
	item_list.erase(item_name);
}

const Array &MeshItemList::get_item_arrays(const StringName &item_name) const
{
	const Array &ret = item_list[item_name].mesh_surfaces;
	return ret;
}

Ref<Mesh> MeshItemList::get_item_mesh(const StringName &item_name) const
{
	return item_list[item_name].mesh;
}

void MeshItemList::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("add_item", "item_name", "mesh"),
			     &MeshItemList::add_item);
	ClassDB::bind_method(D_METHOD("remove_item", "item_name"),
			     &MeshItemList::remove_item);
	ClassDB::bind_method(D_METHOD("get_item_arrays", "item_name"),
			     &MeshItemList::get_item_arrays);
	ClassDB::bind_method(D_METHOD("get_item_mesh", "item_name"),
			     &MeshItemList::get_item_mesh);
}

void GenCitySet::set_guildhall_building_set(const Ref<GenBuildingSet> set)
{
	guildhall_building_set = set;
}

Ref<GenBuildingSet> GenCitySet::get_guildhall_building_set() const
{
	return guildhall_building_set;
}
void GenCitySet::set_court_building_set(const Ref<GenBuildingSet> set)
{
	court_building_set = set;
}
Ref<GenBuildingSet> GenCitySet::get_court_building_set() const
{
	return court_building_set;
}
void GenCitySet::set_road_set(const Ref<GenRoadSet> set)
{
	road_set = set;
}
Ref<GenRoadSet> GenCitySet::get_road_set() const
{
	return road_set;
}
void GenCitySet::set_building_sets(const Array &sets)
{
	building_sets = sets;
}
const Array &GenCitySet::get_building_sets() const
{
	return building_sets;
}
void GenCitySet::set_center_radius(float radius)
{
	center_radius = radius;
}
float GenCitySet::get_center_radius() const
{
	return center_radius;
}
void GenCitySet::set_radius(float radius)
{
	this->radius = radius;
}
float GenCitySet::get_radius() const
{
	return radius;
}
void GenCitySet::set_min_buildings(int minb)
{
	min_buildings = minb;
}
int GenCitySet::get_min_buildings() const
{
	return min_buildings;
}
void GenCitySet::set_max_buildings(int maxb)
{
	max_buildings = maxb;
}
int GenCitySet::get_max_buildings() const
{
	return max_buildings;
}
Dictionary GenCitySet::get_items() const
{
	Dictionary ret;
	int i;
	Vector<Ref<Resource> > sets;
	sets.push_back(guildhall_building_set);
	sets.push_back(court_building_set);
	for (i = 0; i < building_sets.size(); i++) {
		Ref<Resource> d = building_sets[i];
		sets.push_back(d);
	}
	for (i = 0; i < sets.size(); i++) {
		if (!sets[i].ptr())
			continue;
		ret[sets[i]->get("house_type")] = sets[i]->get("items");
	}
	return ret;
}
/*
void GenCitySet::_set_data(const Dictionary &data)
{
	ERR_FAIL_COND(!data.has("gh_building_set"]);
	ERR_FAIL_COND(!data.has("ct_building_set"]);
	ERR_FAIL_COND(!data.has("building_sets"]);
	ERR_FAIL_COND(!data.has("center_radius"]);
	ERR_FAIL_COND(!data.has["radius"]);
	ERR_FAIL_COND(!data.has["min_buildings"]);
	ERR_FAIL_COND(!data.has["max_buildings"]);

	guildhall_building_set = data["gh_building_set"];
	court_building_set = data["ct_building_set"];
	building_sets = data["building_sets"];
	center_radius = data["center_radius"];
	radius = data["radius"];
	min_buildings = data["min_buildings"];
	max_buildings = data["max_buildings"];
}
Dictionary GenCitySet::_get_data() const
{
	Dictionary x;
	x["gh_building_set"] = guildhall_building_set;
	x["ct_building_set"] = court_building_set;
	x["building_sets"] = building_sets;
	x["center_radius"] = center_radius;
	x["radius"] = radius;
	x["min_buildings"] = min_buildings;
	x["max_buildings"] = max_buildings;
	return x;
}
*/

void GenCitySet::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_guildhall_building_set", "set"), &GenCitySet::set_guildhall_building_set);
	ClassDB::bind_method(D_METHOD("get_guildhall_building_set"), &GenCitySet::get_guildhall_building_set);

	ClassDB::bind_method(D_METHOD("set_court_building_set", "set"), &GenCitySet::set_court_building_set);
	ClassDB::bind_method(D_METHOD("get_court_building_set"), &GenCitySet::get_court_building_set);

	ClassDB::bind_method(D_METHOD("set_road_set", "set"), &GenCitySet::set_road_set);
	ClassDB::bind_method(D_METHOD("get_road_set"), &GenCitySet::get_road_set);

	ClassDB::bind_method(D_METHOD("set_building_sets", "set"), &GenCitySet::set_building_sets);
	ClassDB::bind_method(D_METHOD("get_building_sets"), &GenCitySet::get_building_sets);

	ClassDB::bind_method(D_METHOD("set_center_radius", "radius"), &GenCitySet::set_center_radius);
	ClassDB::bind_method(D_METHOD("get_center_radius"), &GenCitySet::get_center_radius);

	ClassDB::bind_method(D_METHOD("set_radius", "radius"), &GenCitySet::set_radius);
	ClassDB::bind_method(D_METHOD("get_radius"), &GenCitySet::get_radius);

	ClassDB::bind_method(D_METHOD("set_min_buildings", "minb"), &GenCitySet::set_min_buildings);
	ClassDB::bind_method(D_METHOD("get_min_buildings"), &GenCitySet::get_min_buildings);

	ClassDB::bind_method(D_METHOD("set_max_buildings", "maxb"), &GenCitySet::set_max_buildings);
	ClassDB::bind_method(D_METHOD("get_max_buildings"), &GenCitySet::get_max_buildings);

	ClassDB::bind_method(D_METHOD("get_items"), &GenCitySet::get_items);
//	ClassDB::bind_method(D_METHOD("_set_data", "data"), &GenCitySet::_set_data);
//	ClassDB::bind_method(D_METHOD("_get_data"), &GenCitySet::_get_data);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "guildhall_building_set", PROPERTY_HINT_RESOURCE_TYPE, "GenBuildingSet"), "set_guildhall_building_set", "get_guildhall_building_set");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "court_building_set", PROPERTY_HINT_RESOURCE_TYPE, "GenBuildingSet"), "set_court_building_set", "get_court_building_set");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "road_set", PROPERTY_HINT_RESOURCE_TYPE, "GenRoadSet"), "set_road_set", "get_road_set");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "building_sets", PROPERTY_HINT_RESOURCE_TYPE, "17/17:GenBuildingSet"), "set_building_sets", "get_building_sets");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "center_radius"), "set_center_radius", "get_center_radius");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "radius"), "set_radius", "get_radius");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "min_buildings"), "set_min_buildings", "get_min_buildings");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_buildings"), "set_max_buildings", "get_max_buildings");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "items"), "", "get_items");
//	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE|PROPERTY_USAGE_NOEDITOR|PROPERTY_USAGE_INTERNAL), "_set_data", "_get_data");
}

GenExteriorSet::GenExteriorSet()
{
	_data["external_wall"] = Ref<ArrayMesh>(NULL);
	_data["external_wall_half"] = Ref<ArrayMesh>(NULL);
	_data["external_doorway"] = Ref<ArrayMesh>(NULL);
	_data["external_window"] = Ref<ArrayMesh>(NULL);
	_data["external_angle"] = Ref<ArrayMesh>(NULL);
	_data["roof_main"] = Ref<ArrayMesh>(NULL);
	_data["roof_side_left"] = Ref<ArrayMesh>(NULL);
	_data["roof_side_right"] = Ref<ArrayMesh>(NULL);
	_data["roof_side_left_block"] = Ref<ArrayMesh>(NULL);
	_data["roof_side_right_block"] = Ref<ArrayMesh>(NULL);
	xmap["xwall"] = "external_wall";
	xmap["xwallh"] = "external_wall_half";
	xmap["xdoor"] = "external_doorway";
	xmap["xwindow"] = "external_window";
	xmap["xangle"] = "external_angle";
	xmap["external_wall"] = "xwall";
	xmap["external_wall_half"] = "xwallh";
	xmap["external_doorway"] = "xdoor";
	xmap["external_window"] = "xwindow";
	xmap["external_angle"] = "xangle";
}
bool GenExteriorSet::_set(const StringName &name, const Variant &property)
{
	if (_data.has(name)) {
		_data[name] = property;
		return true;
	}
	return false;
}
bool GenExteriorSet::_get(const StringName &name, Variant &property) const
{
	if(_data.has(name)) {
		property = _data[name];
		return true;
	} else if (name == "items") {
		Dictionary ret;
		List<String> keys;
		_data.get_key_list(&keys);
		List<String>::Element *e;
		for (e = keys.front(); e; e = e->next()) {
			String k = e->get();
			Dictionary d;
			d["mesh"] = _data[k];
			if (xmap.has(k))
				ret[xmap[k]] = d;
			else
				ret[k] = d;
		}
		property = ret;
		return true;
	}
	return false;
}
void GenExteriorSet::_get_property_list(List<PropertyInfo> *plist) const
{
	List<String> keys;
	_data.get_key_list(&keys);
	List<String>::Element *e;

	for (e = keys.front(); e; e = e->next()) {
		plist->push_back(PropertyInfo(Variant::OBJECT, e->get(), PROPERTY_HINT_RESOURCE_TYPE, "ArrayMesh", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE));
	}
	plist->push_back(PropertyInfo(Variant::DICTIONARY, "items", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE));
}

GenInteriorSet::GenInteriorSet()
{
	_data["wall"] = Ref<ArrayMesh>(NULL);
	_data["wall_half"] = Ref<ArrayMesh>(NULL);
	_data["doorway"] = Ref<ArrayMesh>(NULL);
	_data["window"] = Ref<ArrayMesh>(NULL);
	_data["internal_angle"] = Ref<ArrayMesh>(NULL);
	_data["interior_floor"] = Ref<ArrayMesh>(NULL);
	_data["interior_ceiling"] = Ref<ArrayMesh>(NULL);
	xmap["iwall"] = "wall";
	xmap["iwallh"] = "wall_half";
	xmap["idoor"] = "doorway";
	xmap["iwindow"] = "window";
	xmap["iangle"] = "internal_angle";
	xmap["floor"] = "interior_floor";
	xmap["ceiling"] = "interior_ceiling";
	xmap["wall"] = "iwall";
	xmap["wall_half"] = "iwallh";
	xmap["doorway"] = "idoor";
	xmap["window"] = "iwindow";
	xmap["internal_angle"] = "iangle";
	xmap["interior_floor"] = "floor";
	xmap["interior_ceiling"] = "ceiling";
}
bool GenInteriorSet::_set(const StringName &name, const Variant &property)
{
	if (_data.has(name)) {
		_data[name] = property;
		return true;
	}
	return false;
}
bool GenInteriorSet::_get(const StringName &name, Variant &property) const
{
	if(_data.has(name)) {
		property = _data[name];
		return true;
	} else if (name == "items") {
		Dictionary ret;
		List<String> keys;
		_data.get_key_list(&keys);
		List<String>::Element *e;
		for (e = keys.front(); e; e = e->next()) {
			String k = e->get();
			Dictionary d;
			d["mesh"] = _data[k];
			if (xmap.has(k))
				ret[xmap[k]] = d;
			else
				ret[k] = d;
		}
		property = ret;
		return true;
	}
	return false;
}
void GenInteriorSet::_get_property_list(List<PropertyInfo> *plist) const
{
	List<String> keys;
	_data.get_key_list(&keys);
	List<String>::Element *e;

	for (e = keys.front(); e; e = e->next()) {
		plist->push_back(PropertyInfo(Variant::OBJECT, e->get(), PROPERTY_HINT_RESOURCE_TYPE, "ArrayMesh", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE));
	}
	plist->push_back(PropertyInfo(Variant::DICTIONARY, "items", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE));
}

GenBuildingSet::GenBuildingSet()
{
	_datas["house_type"] = String();
	_datai["min_wings"] = 1;
	_datai["max_wings"] = 1;
	_datai["min_wing_size_x"] = 4;
	_datai["min_wing_size_z"] = 4;
	_datai["max_wing_size_x"] = 8;
	_datai["max_wing_size_z"] = 8;
	_dataf["pwindow"] = 0.6f;
	_dataf["pmidwall"] = 0.2f;
	_dataf["wing_offset"] = 0.0f;
	_datar["exterior_set"] = Ref<GenExteriorSet>(NULL);
	_datar["interior_set"] = Ref<GenInteriorSet>(NULL);
	_datar["furniture_set"] = Ref<GenFurnitureSet>(NULL);
}
bool GenBuildingSet::_set(const StringName &name, const Variant &property)
{
	if (_datas.has(name)) {
		_datas[name] = property;
		return true;
	} else if (_datai.has(name)) {
		_datai[name] = property;
		return true;
	} else if (_dataf.has(name)) {
		_dataf[name] = property;
		return true;
	} else if (_datar.has(name)) {
		_datar[name] = property;
		return true;
	}
	return false;
}
bool GenBuildingSet::_get(const StringName &name, Variant &property) const
{
	if (_datas.has(name)) {
		property = _datas[name];
		return true;
	} else if (_datai.has(name)) {
		property = _datai[name];
		return true;
	} else if (_dataf.has(name)) {
		property = _dataf[name];
		return true;
	} else if (_datar.has(name)) {
		property = _datar[name];
		return true;
	} else if (name == "items") {
		Dictionary ret;
		List<String> res;
		res.push_back("exterior_set");
		res.push_back("interior_set");
		res.push_back("furniture_set");
		List<String>::Element *rd;
		for (rd = res.front(); rd; rd = rd->next()) {
			List<Variant> keys;
			Ref<Resource> mr =  _datar[rd->get()];
			if (!mr.ptr())
				continue;
			Dictionary md = mr->get("items");
			if (md.empty())
				continue;
			md.get_key_list(&keys);
			List<Variant>::Element *e;
			for (e = keys.front(); e; e = e->next()) {
				String k = e->get();
				ret[k] = md[k];
			}
		}
		property = ret;
		return true;
	}
	return false;
}
void GenBuildingSet::_get_property_list(List<PropertyInfo> *plist) const
{
	List<String> keys;
	_datas.get_key_list(&keys);
	_datai.get_key_list(&keys);
	_dataf.get_key_list(&keys);
	_datar.get_key_list(&keys);
	List<String>::Element *e;
	HashMap<String, String> hints;
	HashMap<String, Variant::Type> types;
	hints["exterior_set"] = "GenExteriorSet";
	hints["interior_set"] = "GenInteriorSet";
	hints["furniture_set"] = "GenFurnitureSet";
	types["house_type"] = Variant::STRING;
	types["min_wings"] = Variant::INT;
	types["max_wings"] = Variant::INT;
	types["min_wing_size_x"] = Variant::INT;
	types["min_wing_size_z"] = Variant::INT;
	types["max_wing_size_x"] = Variant::INT;
	types["max_wing_size_z"] = Variant::INT;
	types["pwindow"] = Variant::REAL;
	types["pmidwall"] = Variant::REAL;
	types["wing_offset"] = Variant::REAL;

	for (e = keys.front(); e; e = e->next()) {
		String pname = e->get();
		if (hints.has(pname))
			plist->push_back(PropertyInfo(Variant::OBJECT, pname,
						PROPERTY_HINT_RESOURCE_TYPE,
						hints[pname],
						PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE));
		else if (types.has(pname))
			plist->push_back(PropertyInfo(types[pname],
						pname, PROPERTY_HINT_NONE,
						"",
						PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE));
	}
	plist->push_back(PropertyInfo(Variant::DICTIONARY, "items", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE));
}

GenRoadSet::GenRoadSet()
{
	_data["road_segment_short"] = Ref<ArrayMesh>(NULL);
	_data["road_segment_long"] = Ref<ArrayMesh>(NULL);
	_data["road_turn_left"] = Ref<ArrayMesh>(NULL);
	_data["road_turn_right"] = Ref<ArrayMesh>(NULL);
	_data["road_turn_left2"] = Ref<ArrayMesh>(NULL);
	_data["road_turn_right2"] = Ref<ArrayMesh>(NULL);
	_data["road_y"] = Ref<ArrayMesh>(NULL);
	_data["road_t"] = Ref<ArrayMesh>(NULL);
	_data["road_x"] = Ref<ArrayMesh>(NULL);
	segment_length = segment_width =
		short_segment_length =
		turn_angle = turn_angle2 = 0.0f;
}

void GenRoadSet::_get_property_list(List<PropertyInfo> *plist) const
{
	List<String> keys;
	_data.get_key_list(&keys);
	List<String>::Element *e;

	for (e = keys.front(); e; e = e->next()) {
		plist->push_back(PropertyInfo(Variant::OBJECT, e->get(), PROPERTY_HINT_RESOURCE_TYPE, "ArrayMesh", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE));
	}
	plist->push_back(PropertyInfo(Variant::REAL, "segment_length", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE));
	plist->push_back(PropertyInfo(Variant::REAL, "short_segment_length", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE));
	plist->push_back(PropertyInfo(Variant::REAL, "turn_angle", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE));
	plist->push_back(PropertyInfo(Variant::REAL, "turn_angle2", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE));
	plist->push_back(PropertyInfo(Variant::DICTIONARY, "items", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE));
}

bool GenRoadSet::_set(const StringName &name, const Variant &property)
{
	if (_data.has(name)) {
		_data[name] = property;
		return true;
	} else if (name == "segment_length") {
		segment_length = property;
		return true;
	} else if (name == "short_segment_length") {
		short_segment_length = property;
		return true;
	} else if (name == "turn_angle") {
		turn_angle = property;
		return true;
	} else if (name == "turn_angle2") {
		turn_angle2 = property;
		return true;
	} else if (name == "segment_width") {
		segment_width = property;
		return true;
	}
	return false;
}
bool GenRoadSet::_get(const StringName &name, Variant &property) const
{
	if(_data.has(name)) {
		property = _data[name];
		return true;
	} else if (name == "items") {
		Dictionary ret;
		List<String> keys;
		_data.get_key_list(&keys);
		List<String>::Element *e;
		for (e = keys.front(); e; e = e->next()) {
			String k = e->get();
			Dictionary d;
			d["mesh"] = _data[k];
			ret[k] = d;
		}
		property = ret;
		return true;
	} else if (name == "segment_length") {
		property = segment_length;
		return true;
	} else if (name == "short_segment_length") {
		property = short_segment_length;
		return true;
	} else if (name == "turn_angle") {
		property = turn_angle;
		return true;
	} else if (name == "turn_angle2") {
		property = turn_angle2;
		return true;
	} else if (name == "segment_width") {
		property = segment_width;
		return true;
	}
	return false;
}
