#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED

#include <string>
#include <chrono>

/**
 * Идентификатор объекта.
 */
typedef std::string Label;

/**
 * Отметка времени.
 */
typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> TimeStamp;

/**
 * Координаты объекта.
 * Каждая координата находится в диапазоне [-1,1].
 */
struct Point {
	double x;
	double y;
};

/**
 * Состояния цели.
 * Цель - это объект, за которым ведется слежение.
 */
enum class State {
	// Цель найдена.
	found,
	// Цель отслеживается.
	leading,
	// Цель потеряна.
	lost
};

#endif // COMMON_HPP_INCLUDED
