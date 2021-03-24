#include "voronoi.h"
#define JC_VORONOI_IMPLEMENTATION
#include "thirdparty/voronoi/src/jc_voronoi.h"
#include <cstdio>
Voronoi *Voronoi::get_singleton()
{
	static Voronoi *mo = NULL;
	if (!mo)
		mo = memnew(Voronoi);
	return mo;
}
static inline Vector2 fill_point(const jcv_point *p)
{
	Vector2 point(p->x, p->y);
	return point;
}
static inline Dictionary fill_graphedge(const jcv_graphedge *hedge)
{
	Dictionary ret;
	printf("neighbor: %p\n", hedge->neighbor);
	if (hedge->neighbor)
		ret["neighbor"] = hedge->neighbor->index;
	else
		ret["neighbor"] = -1;
	ret["a"] = fill_point(&hedge->pos[0]);
	ret["b"] = fill_point(&hedge->pos[1]);
	return ret;
}

static inline void sort_site(Dictionary &site)
{
	struct polygon {
		Vector2 center;
		_FORCE_INLINE_ bool operator()(const Vector2 &a, const Vector2 &b) const {
			float a1 = (a - center).angle();
			float a2 = (a - center).angle();
			if (a1 < 0)
				a1 += 2.0 * Math_PI;
			if (a2 < 0)
				a2 += 2.0 * Math_PI;
			return (a1 < a2);
		}
		void prep(const Vector<Vector2> &data)
		{
			int i;
			if (data.size() > 0) {
				for (i = 0; i < data.size(); i++)
					center += data[i];
				center /= (float)data.size();
			}
		}
	};
	Vector<Vector2> region_data;
	Vector<Vector2> vertices = site["vertices"].duplicate();
	SortArray<Vector2, struct polygon> sorter;
	sorter.compare.center = site["pos"];
	sorter.compare.prep(vertices);
	sorter.sort(vertices.ptrw(), vertices.size());
	site["polygon"] = vertices;
}
static inline int vertices_get_index(const Vector<Vector2> &vertices, const Vector2 &pt)
{
	const Vector2 *ptr = vertices.ptr();
	int i;
	for (i = 0; i < vertices.size(); i++) {
		if (ptr[i].distance_squared_to(pt) < 0.1f * 0.1f)
			return i;
	}
	return -1;
}

static inline int vertices_get_index(const Array &vertices, const Vector2 &pt)
{
	int i;
	for (i = 0; i < vertices.size(); i++) {
		const Vector2 &v = vertices[i];
		if (v.distance_squared_to(pt) < 0.1f * 0.1f)
			return i;
	}
	return -1;
}


static inline int vertices_get_index(const PoolVector<Vector2> &vertices, const Vector2 &pt)
{
	const Vector2 *ptr = vertices.read().ptr();
	int i;
	for (i = 0; i < vertices.size(); i++) {
		if (ptr[i].distance_squared_to(pt) < 0.1f * 0.1f)
			return i;
	}
	return -1;
}

static inline Dictionary fill_diagram_dict(const jcv_diagram *diagram)
{
	Dictionary ret;
	HashMap<uint64_t, int> edge_data;
	int point_id_count = 0, edge_id_count = 0, i;
	const jcv_edge *edge = jcv_diagram_get_edges(diagram);
	Dictionary ret_edges;
	while(edge) {
		int id;
		if (jcv_point_eq(&edge->pos[0], &edge->pos[1])) {
			edge = edge->next;
			continue;
		}
		Dictionary e;
		if (edge_data.has((uint64_t)edge))
			id = edge_data[(uint64_t)edge];
		else {
			id = edge_id_count++;
			edge_data[(uint64_t)edge] = id;
		}
		Vector2 p1 = fill_point(&edge->pos[0]);
		Vector2 p2 = fill_point(&edge->pos[1]);
		e["a"] = p1;
		e["b"] = p2;
		e["id"] = id;
		ret_edges[id] = e;
		edge = edge->next;
	}
	Array sites;
	for (i = 0; i < diagram->numsites; i++) {
    		const jcv_site* dsites = jcv_diagram_get_sites(diagram);
		const jcv_site *site = &dsites[i];
		jcv_graphedge *hedge = site->edges;
		Array ges;
		Vector<Vector2> vertices;
		Dictionary s;
		while (hedge) {
			Dictionary ge = fill_graphedge(hedge);
			ge["edge"] = edge_data[(uint64_t)hedge->edge];
			if (vertices_get_index(vertices, ge["a"]) < 0)
				vertices.push_back(ge["a"]);
			if (vertices_get_index(vertices, ge["b"]) < 0)
				vertices.push_back(ge["b"]);
			ge["a"] = vertices_get_index(vertices, ge["a"]);
			ge["b"] = vertices_get_index(vertices, ge["b"]);
			ges.push_back(ge);
			hedge = hedge->next;
		}
		s["index"] = site->index;
		s["pos"] = fill_point(&site->p);
		s["graphedges"] = ges;
		s["vertices"] = vertices;
		sort_site(s);
		sites.push_back(s);
	}
	ret["edges"] = ret_edges;
	ret["sites"] = sites;
	return ret;
}

Dictionary Voronoi::generate_diagram(const PoolVector<Vector2> &points, int relaxations)
{
	int i;
	printf("alive 1\n");
       	jcv_diagram diagram;
	Dictionary ret;
	Vector<jcv_point> _points;
	PoolVector<Vector2> ret_points;
	_points.resize(points.size());
	ret_points.resize(points.size());
	for (i = 0; i < points.size(); i++) {
		_points.write[i].x = points.read()[i].x;
		_points.write[i].y = points.read()[i].y;
	}
	for (i = 0; i < relaxations; i++) {
        	memset(&diagram, 0, sizeof(jcv_diagram));
		jcv_diagram_generate(_points.size(), _points.ptr(), NULL, NULL, &diagram);
		relax_points(&diagram, _points.ptrw());
		jcv_diagram_free(&diagram);
	}
	for (i = 0; i < points.size(); i++) {
		ret_points.write()[i].x = _points[i].x;
		ret_points.write()[i].y = _points[i].y;
	}
	printf("alive 2\n");
	memset(&diagram, 0, sizeof(jcv_diagram));
	jcv_diagram_generate(_points.size(), _points.ptr(), NULL, NULL, &diagram);
	ret = fill_diagram_dict(&diagram);
	ret["points"] = ret_points;
	jcv_diagram_free(&diagram);
	printf("alive 5\n");
	return ret;
}

void Voronoi::relax_points(const jcv_diagram* diagram, jcv_point* points)
{
    const jcv_site* sites = jcv_diagram_get_sites(diagram);
    for( int i = 0; i < diagram->numsites; ++i )
    {
        const jcv_site* site = &sites[i];
        jcv_point sum = site->p;
        int count = 1;

        const jcv_graphedge* edge = site->edges;

        while( edge )
        {
            sum.x += edge->pos[0].x;
            sum.y += edge->pos[0].y;
            ++count;
            edge = edge->next;
        }

        points[site->index].x = sum.x / count;
        points[site->index].y = sum.y / count;
    }
}


void Voronoi::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("generate_diagram", "points", "relaxations"),
			     &Voronoi::generate_diagram);
}

