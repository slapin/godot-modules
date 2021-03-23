#include <cassert>
#include <cmath>
#include <core/variant.h>
#include <core/math/geometry.h>
#include "town_queue.h"

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

void TownQueue::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("produce_item", "item_name", "parent", "extra"),
			     &TownQueue::produce_item);
	ClassDB::bind_method(D_METHOD("produce_item_positional", "item_name", "parent", "position", "rotation", "extra"),
			     &TownQueue::produce_item);
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
	ClassDB::bind_method(D_METHOD("process", "allocate_space", "aabb", "rotation", "patch"),
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

