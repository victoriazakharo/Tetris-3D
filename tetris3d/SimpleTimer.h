#pragma once
#include "Windows.h"
#include <vector>

struct Thunk {};

struct Function {	
	typedef void(Thunk::*InstanceHandler)();
	typedef void(*StaticHandler)();
	union FuncPointer {
		InstanceHandler instanceHandler;
		StaticHandler staticHandler;
	} fp;
	Thunk *trg;
	void Call() const;
};

class TimerCallback
{
	double delta;
	LONGLONG delay;
	LARGE_INTEGER oldTime;
	Function action;
public:
	TimerCallback(double d, Function hadler);
	void UpdateDelay(LARGE_INTEGER new_time);
	void UpdateTime(LARGE_INTEGER new_time);
	void Call() const;
	LONGLONG GetUpdateTime() const;
};

class SimpleTimer
{		
	LARGE_INTEGER newTime;
	std::vector<TimerCallback> callbacks;
public:
	void SetTimer(double delta, void (*fn)());
	template <class Owner>
	void SetTimer(double delta, void (Owner::*fn)(), Owner* This);
	void Initialize();
	void Update();
};

template <class Owner>
void SimpleTimer::SetTimer(double delta, void( Owner::* fn)(), Owner* This)
{
	Function function;
	function.trg = (Thunk*)(This);
	typedef void(Thunk::*XFunc)();
	function.fp.instanceHandler = (XFunc)(fn);
	callbacks.push_back(TimerCallback(delta, function));
}
