#include "queue.h"
#include <modules/voronoi/voronoi.h>
#include <core/math/random_number_generator.h>

class TownQueue: public Reference {
	GDCLASS(TownQueue, Reference)
protected:
	DelayedQueue queue;
	Ref<RandomNumberGenerator> rnd;
	static void _bind_methods();
	Dictionary global_meta;
	int relaxations, seed;
	float center_radius, radius;
	struct patch {
		PoolVector<Vector3> polygon;
		PoolVector<Vector2> polygon2d;
		Vector<Vector2> polycmp;
		int id;
		Vector3 position;
		PoolVector<uint8_t> alloc_grid;
		AABB surroundings;
		bool internal;
		void calculate()
		{
			int i, j;
			surroundings.set_position(position);
			for (i = 0; i < polygon.size(); i++)
				surroundings.expand_to(polygon.read()[i]);
			int x = (int)((surroundings.size.x + 8.0f) / 16.0f);
			int y = (int)((surroundings.size.x + 8.0f) / 16.0f);
			alloc_grid.resize(x * y / 8);
			for (i = 0; i < y; i++)
				for (j = 0; j < x; j++) {
					int data_pos = i * x + j;
					int bytepos = (data_pos >> 3);
					int bit = (1 << (j & 3)); 
					Vector3 pos = surroundings.position + Vector3((float)j * 16.0f + 8.0f, 0.1f, (float)i * 16.0f + 8.0f);
				}
		}
		Dictionary get() const
		{
			Dictionary r;
			r["polygon"] = polygon;
			r["polygon2d"] = polygon2d;
			r["position"] = position;
			r["internal"] = internal;
			r["id"] = id;
			return r;
		}
		patch(const Array &points2d, const Vector2 &pos, int pid, bool qinternal = true)
		{
			int i;
			position = Vector3(pos.x, 0, pos.y);
			polygon2d.resize(points2d.size());
			polygon.resize(points2d.size());
			for (i = 0; i < polygon.size(); i++) {
				Vector2 v = points2d[i];
				polygon.write()[i] = Vector3(v.x, 0.0f, v.y);
				polygon2d.write()[i] = v;
			}
			internal = qinternal;
			id = pid;
		}
		patch(const Vector<Vector2> &points2d, const Vector2 &pos, int pid, bool qinternal = true)
		{
			int i;
			position = Vector3(pos.x, 0, pos.y);
			polygon2d.resize(points2d.size());
			polygon.resize(points2d.size());
			for (i = 0; i < polygon.size(); i++) {
				Vector2 v = points2d[i];
				polygon.write()[i] = Vector3(v.x, 0.0f, v.y);
				polygon2d.write()[i] = v;
			}
			internal = qinternal;
			id = pid;
		}
		patch(const PoolVector<Vector2> &points2d, const Vector2 &pos, int pid, bool qinternal = true)
		{
			int i;
			position = Vector3(pos.x, 0, pos.y);
			polygon2d = points2d;
			polygon.resize(points2d.size());
			for (i = 0; i < polygon.size(); i++) {
				Vector2 v = points2d[i];
				polygon.write()[i] = Vector3(v.x, 0.0f, v.y);
			}
			internal = qinternal;
			id = pid;
		}
		patch()
		{
			internal = true;
		}
	};
	Vector<struct patch> patches;
	bool allocate_space(const AABB& aabb, float rotation, const Dictionary &patch);
public:
	Dictionary produce_item(const StringName &item_name,
			const Dictionary &parent,
			const Dictionary &extra);
	Dictionary produce_item_positional(const StringName &item_name,
			const Dictionary &parent,
			const Vector3 &position,
			float rotation,
			const Dictionary &extra);
	inline void push_back(const Dictionary &data)
	{
		queue.push_back(data);
	}
	inline void push_front(const Dictionary &data)
	{
		queue.push_front(data);
	}
	inline Dictionary pop_front()
	{
		return queue.pop_front();
	}
	inline void push_back_delayed(const Dictionary &data)
	{
		queue.push_back_delayed(data);
	}
	inline void register_callback(const StringName &item_name, Object *obj, const StringName &func)
	{
		queue.register_callback(item_name, obj, func);
	}
	inline void register_default_callback(Object *obj, const StringName &func)
	{
		queue.register_default_callback(obj, func);
	}
	inline void unregister_callback(const StringName &item_name)
	{
		queue.unregister_callback(item_name);
	}
	inline void process()
	{
		queue.process();
	}
	void startup(const Dictionary &town_extra);
	void handle_town(const Dictionary &item);
	void handle_patch(const Dictionary &item);
	TownQueue();
};

