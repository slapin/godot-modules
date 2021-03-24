#include <core/reference.h>
struct _jcv_diagram;
struct _jcv_point;
typedef struct _jcv_diagram     jcv_diagram;
typedef struct _jcv_point       jcv_point;

class Voronoi: public Object {
	GDCLASS(Voronoi, Object)
protected:
	static void _bind_methods();
	static void relax_points(const jcv_diagram* diagram, jcv_point* points);
public:
	Dictionary generate_diagram(const PoolVector<Vector2> &points, int relaxations);
	static Voronoi *get_singleton();
};

