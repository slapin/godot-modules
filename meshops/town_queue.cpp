#include <cassert>
#include <cmath>
#include <core/variant.h>
#include <core/math/geometry.h>
#include "town_queue.h"
#include "town.h"

static inline AABB cut_left(AABB &aabb, float amount)
{
	if (amount > aabb.size.x)
		return AABB();
	AABB ret(aabb.position, Vector3(amount, aabb.size.y, aabb.size.z));
	aabb.position.x += amount;
	aabb.size.x -= amount;
	return ret;
}

static inline AABB cut_ztop(AABB &aabb, float amount)
{
	if (amount > aabb.size.z)
		return AABB();
	AABB ret(aabb.position, Vector3(aabb.size.x, aabb.size.y, amount));
	aabb.position.z += amount;
	aabb.size.z -= amount;
	return ret;
}

static inline Variant get_item_data(const Dictionary &item, const char *key)
{
	const Dictionary &item_data = item["data"];
	return item_data[key];
}

static inline void set_item_data(Dictionary &item, const char *key, const Variant &value)
{
	if (!item.has("data"))
		item["data"] = Dictionary();
	Dictionary data = item["data"];
	data[key] = value;
}

static inline void set_item_data(Dictionary &item, const StringName &key, const Variant &value)
{
	if (!item.has("data"))
		item["data"] = Dictionary();
	Dictionary data = item["data"];
	data[key] = value;
}

static inline String get_item_name(const Dictionary &item)
{
	return item["name"];
}

static inline void set_item_name(Dictionary &item, const StringName &value)
{
	item["name"] = value;
}

static inline bool item_has_data(const Dictionary &item, const char *key)
{
	const Dictionary &item_data = item["data"];
	return item_data.has(key);
}

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

Dictionary TownQueue::produce_item(const StringName &item_name,
		const Dictionary &parent,
		const Dictionary &extra)
{
	Dictionary item;
	const char *copy_fields[] = {
		"set",
		"town",
		"patch",
		"lot",
		"wing"
	};
	set_item_name(item, item_name);
	item["data"] = Dictionary();

	if (!parent.empty()) {
		int j;
		const String &parent_name = parent["name"];
		set_item_data(item, parent_name, parent);
		for (j = 0; j < (int)ARRAY_SIZE(copy_fields); j++)
			if (item_has_data(parent, copy_fields[j]))
				set_item_data(item,
						copy_fields[j],
						get_item_data(parent,
							copy_fields[j]));
	}
	const Variant *e;
	for (e = extra.next(NULL); e; e = extra.next(e))
		set_item_data(item, *e, extra[*e]);
	return item;
}

void TownQueue::startup(const Dictionary &town_extra)
{
	rnd.instance();
	seed = -1;
	radius = 0.0f;
	center_radius = 0.0f;
	if (town_extra.has("seed"))
		seed = town_extra["seed"];
	if (town_extra.has("radius"))
		radius = town_extra["radius"];
	if (town_extra.has("center_radius"))
		center_radius = town_extra["center_radius"];
	if (seed < 0)
		rnd->randomize();
	seed = rnd->get_seed();
	global_meta["seed"] = seed;
	global_meta["radius"] = radius;
	global_meta["center_radius"] = center_radius;
	global_meta["lots"] = Array();
	global_meta["road_segments"] = Array();
	global_meta["road_intersections"] = Array();
	Dictionary town_item = produce_item_positional("town", Dictionary(), Vector3(), 0.0f, town_extra);
	queue.push_back(town_item);
	queue.register_callback_native("town", this, (void (Object::*)(const Dictionary&)) &TownQueue::handle_town);
	queue.register_callback_native("patch", this, (void (Object::*)(const Dictionary&)) &TownQueue::handle_patch);
	queue.register_callback_native("lot", this, (void (Object::*)(const Dictionary&)) &TownQueue::handle_lot);
	queue.register_callback_native("residence", this, (void (Object::*)(const Dictionary&)) &TownQueue::handle_residence);
	queue.register_callback_native("farm", this, (void (Object::*)(const Dictionary&)) &TownQueue::handle_farm);
}

void TownQueue::init_grid(Dictionary &item)
{
	int i,j;
	int grid_x = get_item_data(item, "grid_x");
	int grid_z = get_item_data(item, "grid_z");
	Vector3 grid_origin = get_item_data(item, "grid_origin"); 
	PoolVector<uint8_t> grid = get_item_data(item, "grid");
	PoolVector<Vector3> polygon = get_item_data(item, "polygon_rot");
	Vector<Vector2> poly2d;
	poly2d.resize(polygon.size());
	const Vector3 *pd = polygon.read().ptr();
	for (i = 0; i < polygon.size(); i++)
		poly2d.write[i] = Vector2(pd[i].x, pd[i].z);
	uint8_t *gptr = grid.write().ptr();
	for (i = 0; i < grid_z; i++)
		for (j = 0; j < grid_x; j++) {
			Vector3 grid_pos = grid_origin + Vector3((float)j * 16.0, 0.0f, (float)i * 16.0);
			if (Geometry::is_point_in_polygon(Vector2(grid_pos.x, grid_pos.z), poly2d))
				gptr[i * grid_x + j] = 0;
			else
				gptr[i * grid_x + j] = 0xff;
		}
	set_item_data(item, "grid", grid);
}

void TownQueue::handle_town(const Dictionary &item)
{
	Voronoi *voronoi = Voronoi::get_singleton();
	int patches_count = 5 + rnd->randi() % 10, i;
	PoolVector<Vector2> points2d;
	points2d.resize(patches_count * 8);
	float sa = rnd->randf() * 2.0 * Math_PI;
	for (i = 0; i < patches_count * 8; i++) {
		float a = sa + sqrtf((float)i) * 5.0f;
		float r = (i == 0) ? 0.0f : MIN(center_radius + (float)i * 4.0f * rnd->randf(), radius);
		float x = cos(a) * r;
		float y = sin(a) * r;
		Vector2 d(x, y);
		points2d.write()[i] = d;
	}
	Dictionary diagram = voronoi->generate_diagram(points2d, relaxations);
	global_meta["voronoi"] = diagram;
	Array sites = diagram["sites"];
	patches.resize(sites.size());
	int plaza_id = -1;
	float plength = Math_INF;
	for (i = 0; i < sites.size(); i++) {
		Dictionary site = sites[i];
		Vector<Vector2> polygon = site["polygon"];
		Vector2 pos = site["pos"];
		printf("site pos %ls\n", String(pos).c_str());
		float l = pos.length_squared();
		if (plength > l) {
			plaza_id = i;
			plength = l;
		}
		struct patch p(polygon, pos, patches.size());
		patches.write[i] = p;
	}
	for (i = 0; i < sites.size(); i++) {
		Dictionary p_extra = patches[i].get();
		if (i == plaza_id)
			p_extra["plaza"] = true;
		else
			p_extra["plaza"] = false;
		Dictionary patch_item = produce_item_positional("patch", item, p_extra["position"], 0.0f, p_extra);
		queue.push_back(patch_item);
	}
}
static inline float polygon_area(const PoolVector<Vector3> &polygon)
{
    float area = 0.0f;
    int n = polygon.size();
    const Vector3 *p = polygon.read().ptr();

    for (int i = 0; i < n; i++)
    {
       int j = (i + 1) % n;
       area += 0.5f * (p[i].x*p[j].z -  p[j].x*p[i].z);
    }
    return (area);
}

static inline bool get_grid_free(const Dictionary &item, int x, int z, int dx, int dz)
{
	int grid_x = get_item_data(item, "grid_x");
	int grid_z = get_item_data(item, "grid_z");
	PoolVector<uint8_t> grid = get_item_data(item, "grid");
	const uint8_t *data = grid.read().ptr();
	int minx = MAX(0, x);
	int maxx = MIN(x + dx, grid_x);
	int minz = MAX(0, z);
	int maxz = MIN(z + dz, grid_z);
	int i, j;
	if (x >= grid_x || z >= grid_z)
		return false;
	for (i = minz; i < maxz; i++)
		for (j = minx; j < maxx; j++)
			if (data[i * grid_x + j] != 0)
				return false;
	return true;
}
static inline bool get_grid_free(const Dictionary &item, const AABB &aabb)
{
	Vector3 grid_origin = get_item_data(item, "grid_origin");
	Vector3 offt = aabb.position - grid_origin;
	int x = (int)((offt.x + 8.0f) / 16.0f);
	int z = (int)((offt.x + 8.0f) / 16.0f);
	int dx = (int)((aabb.size.x + 8.0f) / 16.0f);
	int dz = (int)((aabb.size.z + 8.0f) / 16.0f);
	return get_grid_free(item, x, z, dx, dz);
}
static inline bool shrink_aabb(const Dictionary &item, AABB &aabb)
{
	while (aabb.size.x > 16.0f &&
			aabb.size.z > 16.0f &&
			!get_grid_free(item, aabb)) {
		aabb.position.x += 1.0f;
		aabb.position.z += 1.0f;
		aabb.size.x -= 2.0f;
		aabb.size.z -= 2.0f;
	}
	if (aabb.size.x > 16.0f && aabb.size.z > 16.0f)
		return true;
	else
		return false;
}

void TownQueue::handle_patch(const Dictionary &item)
{
	int i;
	Dictionary item_data = item["data"];
	Dictionary p_extra;
	p_extra["plaza"] = item_data["plaza"];
	float rotation = rnd->randf() * 2.0f * Math_PI;
	Transform xform = Transform().rotated(Vector3(0, 1, 0), rotation);
	PoolVector<Vector3> polygon = item_data["polygon"];
	PoolVector<Vector3> polygon_rot;
	polygon_rot.resize(polygon.size());
	Vector3 position = item_data["position"];
	AABB aabb_rot(position, Vector3()), aabb(position, Vector3());
	for (i = 0; i < polygon.size(); i++) {
		aabb.expand_to(polygon.read()[i]);
		Vector3 xv = xform.xform_inv(polygon.read()[i]);
		polygon_rot.write()[i] = xv;
		aabb_rot.expand_to(xv);
	}
	aabb.size.y = 3.0f;
	aabb_rot.size.y = 3.0f;
	AABB aabb_shrunk = aabb_rot;
	Vector3 grid_origin = aabb_rot.position;
	int grid_x = (int)(aabb_rot.size.x + 8.0f / 16.0f);
	int grid_z = (int)(aabb_rot.size.z + 8.0f / 16.0f);
	PoolVector<uint8_t> grid;
	grid.resize(grid_x * grid_z);
	p_extra["polygon"] = polygon;
	p_extra["polygon_rot"] = polygon_rot;
	p_extra["aabb"] = aabb;
	p_extra["aabb_rot"] = aabb_rot;
	p_extra["grid_origin"] = grid_origin;
	p_extra["grid_x"] = grid_x;
	p_extra["grid_z"] = grid_z;
	p_extra["grid"] = grid;
	p_extra["area"] = polygon_area(polygon);
	Dictionary lot = produce_item_positional("lot", item, item_data["position"], rotation, p_extra);
	init_grid(lot);
	if (shrink_aabb(lot, aabb_shrunk))
		set_item_data(lot, "aabb_rot_shrunk", aabb_shrunk);
	else
		set_item_data(lot, "aabb_rot_shrunk", AABB());
	queue.push_back(lot);
	printf("grid_x %d grid_z %d\n", grid_x, grid_z);
}
void TownQueue::handle_lot(const Dictionary &item)
{
	int grid_x = get_item_data(item, "grid_x");
	int grid_z = get_item_data(item, "grid_z");
	AABB aabb = get_item_data(item, "aabb_rot_shrunk");
	Vector3 position = get_item_data(item, "position");
	float rotation = get_item_data(item, "rotation");
	HashMap<int, String> item_selection;
	List<int> key_list;
	item_selection[80] = "industry";
	item_selection[60] = "farm";
	item_selection[40] = "residence";
	item_selection[20] = "recreation";
	item_selection.get_key_list(&key_list);
	key_list.sort();
	key_list.invert();
	String item_name = "farm";
	if (grid_x > 128 && grid_z > 128)
		item_name = "farm";
	else if (grid_x > 64 && grid_z > 64) {
		List<int>::Element *k = key_list.front();
		int choice = rnd->randi() % 100;
		while(k) {
			int key = k->get();
			if (choice > key) {
				item_name = item_selection[key];
				break;
			}
			k = k->next();
		}
	}
	else if (grid_x <= 64 && grid_z <= 64)
		item_name = "residence";
	Dictionary next_item = produce_item_positional(item_name, item, position, rotation, item["data"]);
	set_item_data(next_item, "aabb", aabb);
	queue.push_back(next_item);
}
void TownQueue::handle_residence(const Dictionary &item)
{
	Dictionary lot = get_item_data(item, "lot");
	GenCitySet *city = Object::cast_to<GenCitySet>(get_item_data(item, "set"));
	const Array &buildings = city->get_building_sets();
	ERR_FAIL_COND(buildings.size() == 0);
	int bset_id = rnd->randi() % buildings.size();
	GenBuildingSet *bset = Object::cast_to<GenBuildingSet>(buildings[bset_id]);
	AABB aabb = get_item_data(item, "aabb");
	if (aabb.size.x < 16.0f || aabb.size.z < 16.0f)
		/* we better produce hut in this case */
		return;
	Dictionary p_extra;
	p_extra["bset"] = bset;
	p_extra["house_type"] = bset->get("house_type");
	p_extra["placement"] = "arc";
	p_extra["aabb"] = aabb;
	Dictionary house = produce_item_positional("house",
			item, get_item_data(item, "position"),
			/* need to remove this as lot is already rotated...,
			 * but need this for house node */
			get_item_data(item, "rotation"),
			p_extra);
	queue.push_back(house);
}
void TownQueue::handle_farm(const Dictionary &item)
{
	AABB aabb = get_item_data(item, "aabb");
	AABB field_aabb, farmtech_aabb, residence_aabb;
	if (aabb.size.x > aabb.size.z)
		field_aabb = cut_left(aabb, aabb.size.x * 0.5f);
	else
		field_aabb = cut_ztop(aabb, aabb.size.z * 0.5f);
	if (aabb.size.x > aabb.size.z)
		farmtech_aabb = cut_left(aabb, aabb.size.x * 0.5f);
	else
		farmtech_aabb = cut_ztop(aabb, aabb.size.z * 0.5f);
	residence_aabb = aabb;
	Vector3 field_position = field_aabb.position +
		Vector3(field_aabb.size.x, 0.0f, field_aabb.size.z) * 0.5f;
	Vector3 farmtech_position = farmtech_aabb.position +
		Vector3(farmtech_aabb.size.x, 0.0f, farmtech_aabb.size.z) * 0.5f;
	Vector3 residence_position = residence_aabb.position +
		Vector3(residence_aabb.size.x, 0.0f, residence_aabb.size.z) * 0.5f;
	Dictionary field_extra, farmtech_extra, residence_extra;
	field_extra["aabb"] = field_aabb;
	farmtech_extra["aabb"] = farmtech_aabb;
	residence_extra["aabb"] = residence_aabb;
	Dictionary field = produce_item_positional("field", item, field_position, 0.0f, field_extra);
	Dictionary farmtech = produce_item_positional("farmtech", item, farmtech_position, 0.0f, farmtech_extra);
	Dictionary residence = produce_item_positional("residence", item, residence_position, 0.0f, residence_extra);
	queue.push_back(field);
	queue.push_back(farmtech);
	queue.push_back(residence);
}

void TownQueue::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("produce_item", "item_name", "parent", "extra"),
			     &TownQueue::produce_item, DEFVAL(Dictionary()), DEFVAL(Dictionary()));
	ClassDB::bind_method(D_METHOD("produce_item_positional", "item_name", "parent", "position", "rotation", "extra"),
			     &TownQueue::produce_item_positional, DEFVAL(0.0f), DEFVAL(Dictionary()));
	ClassDB::bind_method(D_METHOD("startup", "town_extra"),
			     &TownQueue::startup);
	ClassDB::bind_method(D_METHOD("push_back", "item"),
			     &TownQueue::push_back);
	ClassDB::bind_method(D_METHOD("push_front", "item"),
			     &TownQueue::push_front);
	ClassDB::bind_method(D_METHOD("pop_front"),
			     &TownQueue::pop_front);
	ClassDB::bind_method(D_METHOD("push_back_delayed", "item"),
			     &TownQueue::push_back_delayed);
	ClassDB::bind_method(D_METHOD("register_callback", "item_name", "obj", "func"),
			     &TownQueue::register_callback);
	ClassDB::bind_method(D_METHOD("register_default_callback", "obj", "func"),
			     &TownQueue::register_default_callback);
	ClassDB::bind_method(D_METHOD("unregister_callback", "item_name"),
			     &TownQueue::unregister_callback);
	ClassDB::bind_method(D_METHOD("process"),
			     &TownQueue::process);
	ClassDB::bind_method(D_METHOD("allocate_space", "aabb", "rotation", "patch"),
			&TownQueue::allocate_space);
}
bool TownQueue::allocate_space(const AABB& aabb, float rotation, const Dictionary &patch)
{
	int i;
	Transform xform = Transform().rotated(Vector3(0, 1.0f, 0), rotation);
	const PoolVector<Vector3> &polygon = patch["polygon"];
	Vector3 size = aabb.get_size();
	Vector3 center = aabb.get_position() + Vector3(size.x, 0.1f, size.z) * 0.5f;
	PoolVector<Vector2> polygon2d = patch["polygon2d"];
	Vector<Vector2> p2d;
	p2d.resize(polygon2d.size());
	for (i = 0; i < polygon2d.size(); i++)
		p2d.write[i] = polygon2d.read()[i];
	if (!Geometry::is_point_in_polygon(Vector2(center.x, center.z), p2d))
		return false;
	for (i = 0; i < polygon.size(); i++) {
		Vector3 p1 = xform.xform_inv(polygon.read()[i]);
		Vector3 p2 = xform.xform_inv(polygon.read()[(i + 1) % polygon.size()]);
		p1.y = 0.1f;
		p2.y = 0.2f;
		if (aabb.intersects_segment(p1, p2))
			return false;
	}
	return true;
}

Dictionary TownQueue::produce_item_positional(const StringName &item_name,
		const Dictionary &parent,
		const Vector3 &position,
		float rotation,
		const Dictionary &extra)
{
	Dictionary item = produce_item(item_name, parent, extra);
	set_item_data(item, "position", position);
	set_item_data(item, "rotation", rotation);
	return item;
}

TownQueue::TownQueue()
{
	relaxations = 3;
}

