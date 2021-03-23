#ifndef FUNCTOR_H
#define FUNCTOR_H
template <class T, class S>
struct functor {
	void *obj;
	typedef void (T::*_memfunc)(S);
	_memfunc func;
	functor(void *obj,  _memfunc func)
	{
		this->obj = obj;
		this->func = func;
	}
	functor()
	{
		obj = 0;
	}
	operator bool() const
	{
		return !!obj;
	}
	void operator()(S item)
	{
		run(*this, item);
	}
	static void run(functor &ftor, S item)
	{
		T *obj = (T *)ftor.obj;
		_memfunc func = ftor.func;
		(obj->*func)(item);
	}
};

#endif

