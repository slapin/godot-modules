#ifndef MESHOPS_H
#define MESHOPS_H
#include <core/reference.h>
#include <scene/resources/mesh.h>
#include <scene/resources/material.h>

class MeshOps: public Reference {
	GDCLASS(MeshOps, Reference)
protected:
	static void _bind_methods();
public:
	static MeshOps *get_singleton();
	Array merge_meshes(const Array &surfaces, const Array &xforms) const;
};

#endif

