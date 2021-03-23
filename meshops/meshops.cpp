#include "meshops.h"

MeshOps *MeshOps::get_singleton()
{
	static MeshOps *mo = NULL;
	if (!mo)
		mo = memnew(MeshOps);
	return mo;
}

Array MeshOps::merge_meshes(const Array &surfaces, const Array &xforms) const
{
	int i;
	Array ret;
	ret.resize(Mesh::ARRAY_MAX);
	int surface_count = surfaces.size();
	PoolVector<int> cur_index;
	PoolVector<Vector3> cur_vertex;
	PoolVector<Vector3> cur_normal;
	PoolVector<Vector2> cur_uv;
	for (i = 0; i < surface_count; i++) {
		int icount, count, j, k, st;
		const Array &sts = surfaces[i];
		for (st = 0; st < sts.size(); st++) {
			const Array &s = sts[st];
			const PoolVector<int> &index = s[Mesh::ARRAY_INDEX];
			const PoolVector<Vector3> &vertex =
				s[Mesh::ARRAY_VERTEX];
			const PoolVector<Vector3> &normal =
				s[Mesh::ARRAY_NORMAL];
			const PoolVector<Vector2> &uv = s[Mesh::ARRAY_TEX_UV];
			icount = cur_index.size();
			count = cur_vertex.size();
			const Transform &xform = xforms[i];
			for (j = 0; j < Mesh::ARRAY_MAX; j++) {
				switch (j) {
				case Mesh::ARRAY_INDEX:
					cur_index.append_array(index);
					for (k = 0; k < index.size(); k++)
						cur_index.write()[k + icount] =
							cur_index.read()[k +
									 icount] +
							count;
					break;
				case Mesh::ARRAY_VERTEX:
					cur_vertex.append_array(vertex);
					for (k = 0; k < vertex.size(); k++)
						cur_vertex.write()[k + count] =
							xform.xform(
								cur_vertex.read()
									[k +
									 count]);
					break;
				case Mesh::ARRAY_NORMAL:
					cur_normal.append_array(normal);
					/* all sizes except index are the same */
					for (k = 0; k < vertex.size(); k++)
						cur_normal.write()[k + count] =
							xform.basis.xform(
								cur_normal.read()
									[k +
									 count]);
					break;
				case Mesh::ARRAY_TEX_UV:
					cur_uv.append_array(uv);
				}
			}
		}
	}
	ret[Mesh::ARRAY_INDEX] = cur_index;
	ret[Mesh::ARRAY_VERTEX] = cur_vertex;
	ret[Mesh::ARRAY_NORMAL] = cur_normal;
	ret[Mesh::ARRAY_TEX_UV] = cur_uv;
	return ret;
}

void MeshOps::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("merge_meshes", "surfaces", "xforms"),
			     &MeshOps::merge_meshes);
}

