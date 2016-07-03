#include "SimpleTimer.h"

void Function::Call() const
{
	return trg == nullptr ? (*fp.staticHandler)() : (trg->*fp.instanceHandler)();
}

TimerCallback::TimerCallback(double d, Function hadler)
{
	delay = {};
	delta = d;
	action = hadler;
}

void TimerCallback::UpdateDelay(LARGE_INTEGER new_time)
{
	delay = new_time.QuadPart * delta;
}

void TimerCallback::UpdateTime(LARGE_INTEGER new_time)
{
	oldTime = new_time;
}

void TimerCallback::Call() const
{
	action.Call();
}

LONGLONG TimerCallback::GetUpdateTime() const
{
	return oldTime.QuadPart + delay;
}

void SimpleTimer::SetTimer(double delta, void(* fn)())
{
	Function function;
	function.fp.staticHandler = fn;
	function.trg = nullptr;
	callbacks.push_back(TimerCallback(delta, function));
}

void SimpleTimer::Initialize()
{
	QueryPerformanceFrequency(&newTime);
	for (auto it = callbacks.begin(); it != callbacks.end(); ++it) {		
		it->UpdateDelay(newTime);
	}
	QueryPerformanceCounter(&newTime);
	for (auto it = callbacks.begin(); it != callbacks.end(); ++it) {
		it->UpdateTime(newTime);
	}
}

void SimpleTimer::Update()
{
	QueryPerformanceCounter(&newTime);
	for (auto it = callbacks.begin(); it != callbacks.end(); ++it) {
		if (newTime.QuadPart > it->GetUpdateTime()) {
			it->UpdateTime(newTime);
			it->Call();
		}
	}
}
