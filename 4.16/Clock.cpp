#include "Clock.h"

#include "FileReader.h"

#include <Windows.h>
#include <GLFW/glfw3.h>

#define FILE_CLOCK_FPS "i_clock_fps"

#define DEFAULT_FPS_LIMIT 60

Clock::Clock(int fps) :
	_cap			( fps ),
	_ms				( 1000.0 / _cap ),
	_is_limit		( _cap > 0 ? true : false ),
	_frames			( 0.0 ),
	_time			( 0.0 ),
	_update_ticks	( 0 ),
	_fms			( 0.0 ),
	_previous_ticks ( glfwGetTime() )
{}

bool Clock::update(double interval) {
	update_time();
	_frames++;

	if (_is_limit && (_time < _ms)) {
		double delay = _ms - _time;
		if (delay > 0.0) {
			Sleep(delay);
			update_time();
		}
	}

	if (_previous_ticks >= _update_ticks) {
		_fms = 1000.0 / _frames;
		_frames = 0;
		_update_ticks += interval;
		return true;
	}

	return false;
}

void Clock::reset() {
	_frames = 0;
	_update_ticks = _previous_ticks + UPDATE_INTERVAL;
	update_time();
}

void Clock::limit(bool limit) {
	_is_limit = limit;
}

void Clock::set_cap(int cap) {
	_cap = cap;
	_ms = 1000.0 / _cap;
}

void Clock::update_time() {
	_ticks = glfwGetTime() - _previous_ticks;
	_previous_ticks = glfwGetTime();
	_time = _ticks * .001;
}

const int Clock::load_cap(const char* file_path) {
	FileReader file(file_path);

	int fps = DEFAULT_FPS_LIMIT;
	file.s_read_int(&fps, FILE_CLOCK_FPS, "System");

	return fps;
}

const double Clock::get_time() {
	return _time;
}

const double Clock::get_fms() {
	return _fms;
}

// Time format (00:00:00:000)
// days : hours : minutes : seconds : miliseconds
const std::string Clock::get_display_time() {
	double miliseconds = glfwGetTime() * .001;
	miliseconds = miliseconds - floor(miliseconds);
	miliseconds *= 1000.0;

	const int ticks = (int)(glfwGetTime() * .001);
	const int seconds = ticks % 60;
	const int minutes = int(ticks / 60) % 60;
	const int hours = int(ticks / 3600) % 24;
	const int days = int(ticks / 86400);

	std::string time = "";

	time += (days > 0) ? std::to_string(days) + ":" : "";
	time += ((hours < 10) ? "0" : "") + std::to_string(hours) + ":";
	time += ((minutes < 10) ? "0" : "") + std::to_string(minutes) + ":";
	time += ((seconds < 10) ? "0" : "") + std::to_string(seconds) + ":";
	time += std::to_string((int)miliseconds);

	return time;
}