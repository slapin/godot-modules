#include <core/engine.h>

#include "register_types.h"
#include "meshops.h"

void register_meshops_types()
{
	ClassDB::register_class<MeshItemList>();
	ClassDB::register_class<GenCitySet>();
	ClassDB::register_class<MeshOps>();
	Engine::get_singleton()->add_singleton(
			Engine::Singleton("MeshOps",
				MeshOps::get_singleton()));
}
void unregister_meshops_types()
{
}

