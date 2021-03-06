#include "meshops.h"

MeshOps * MeshOps::get_singleton()
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
			const PoolVector<Vector3> &vertex = s[Mesh::ARRAY_VERTEX];
			const PoolVector<Vector3> &normal = s[Mesh::ARRAY_NORMAL];
			const PoolVector<Vector2> &uv = s[Mesh::ARRAY_TEX_UV];
			icount = cur_index.size();
			count = cur_vertex.size();
			const Transform &xform = xforms[i];
			for (j = 0; j < Mesh::ARRAY_MAX; j++) {
				switch(j) {
				case Mesh::ARRAY_INDEX:
					cur_index.append_array(index);
					for (k = 0; k < index.size(); k++)
						cur_index.write()[k + icount] =
							cur_index.read()[k + icount] + count;
					break;
				case Mesh::ARRAY_VERTEX:
					cur_vertex.append_array(vertex);
					for (k = 0; k < vertex.size(); k++)
						cur_vertex.write()[k + count] = xform.xform(cur_vertex.read()[k + count]);
					break;
				case Mesh::ARRAY_NORMAL:
					cur_normal.append_array(normal);
					/* all sizes except index are the same */
					for (k = 0; k < vertex.size(); k++)
						cur_normal.write()[k + count] = xform.basis.xform(cur_normal.read()[k + count]);
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
void MeshItemList::remove_item(const StringName &item_type, const StringName &item_name)
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

void MeshOps::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("merge_meshes", "surfaces", "xforms"),
		&MeshOps::merge_meshes);
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

void GenCitySet::set_guildhall_building_set(const Ref<Resource> set)
{
}

void GenCitySet::_bind_methods()
{
}
