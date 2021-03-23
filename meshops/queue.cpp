#include <cstdio>
#include "queue.h"

DelayedQueue::DelayedQueue()
{
	default_func = "";
	default_obj = NULL;
}

void DelayedQueue::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("push_back", "item"),
			     &DelayedQueue::push_back);
	ClassDB::bind_method(D_METHOD("push_front", "item"),
			     &DelayedQueue::push_front);
	ClassDB::bind_method(D_METHOD("pop_front"),
			     &DelayedQueue::pop_front);
	ClassDB::bind_method(D_METHOD("push_back_delayed", "item"),
			     &DelayedQueue::push_back_delayed);
	ClassDB::bind_method(D_METHOD("register_callback", "item_name", "obj", "func"),
			     &DelayedQueue::register_callback);
	ClassDB::bind_method(D_METHOD("register_default_callback", "obj", "func"),
			     &DelayedQueue::register_default_callback);
	ClassDB::bind_method(D_METHOD("unregister_callback", "item_name"),
			     &DelayedQueue::unregister_callback);
	ClassDB::bind_method(D_METHOD("process"),
			     &DelayedQueue::process);
}

void DelayedQueue::push_back(const Dictionary &data)
{
	queue.push_back(data);
}

void DelayedQueue::push_front(const Dictionary &data)
{
	queue.push_front(data);
}

Dictionary  DelayedQueue::pop_front()
{
	Dictionary ret = queue[0];
	queue.pop_front();
	return ret;
}

void DelayedQueue::push_back_delayed(const Dictionary &data)
{
	queue_delayed.push_back(data);
}

void DelayedQueue::register_callback(const StringName &item_name, Object *obj, const StringName &func)
{
	struct _reg r;
	r.name = item_name;
	r.obj = obj;
	r.func = func;
	reg[item_name] = r;
}
void DelayedQueue::register_default_callback(Object *obj, const StringName &func)
{
	default_obj = obj;
	default_func = func;
}
void DelayedQueue::unregister_callback(const StringName &item_name)
{
	reg.erase(item_name);
}

void DelayedQueue::process()
{
	while (queue.size() > 0 || queue_delayed.size() > 0) {
		if (queue.size() == 0) {
			queue.push_back(queue_delayed[0]);
			queue_delayed.pop_front();
		}
		const Dictionary &item = queue[0];
		const String &name = item["name"];
		if (callbacks.has(name)) {
			functor<Object, const Dictionary &> f = callbacks[name];
			f(item);
		}
		else if (reg.has(name))
			reg[name].obj->call(reg[name].func, item);
		else {
			if (default_callback) {
				functor<Object, const Dictionary &> f = default_callback;
				f(item);
			}
			if (default_obj)
				default_obj->call(default_func, item);
		}
		queue.pop_front();
	}
}

