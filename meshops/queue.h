#ifndef QUEUE_H
#define QUEUE_H
#include <core/reference.h>
#include <core/object.h>
#include "functor.h"
class DelayedQueue: public Reference {
	GDCLASS(DelayedQueue, Reference)
protected:
	List<Dictionary> queue;
	List<Dictionary> queue_delayed;
	static void _bind_methods();
	struct _reg {
		String name;
		Object * obj;
		String func;
	};
	HashMap<String, struct _reg> reg;
	String default_func;
	Object *default_obj;
	HashMap<String, functor<Object, const Dictionary &> > callbacks;
	functor<Object, const Dictionary &> default_callback;

public:
	void push_back(const Dictionary &data);
	void push_front(const Dictionary &data);
	Dictionary pop_front();
	void push_back_delayed(const Dictionary &data);
	void register_callback(const StringName &item_name, Object *obj, const StringName &func);
	void register_callback_native(const StringName &item_name, Object *obj, void(Object::*func)(const Dictionary &item))
	{
		functor<Object, const Dictionary &> f(obj, func);
		callbacks[item_name] = f;
	}
	void register_default_callback_native(Object *obj, void(Object::*func)(const Dictionary& item))
	{
		functor<Object, const Dictionary &> f(obj, func);
		default_callback = f;
	}
	void register_default_callback(Object *obj, const StringName &func);
	void unregister_callback(const StringName &item_name);
	void process();
	DelayedQueue();
};
#endif

