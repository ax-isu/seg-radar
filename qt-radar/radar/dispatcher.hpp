#ifndef DISPATCHER_HPP_INCLUDED
#define DISPATCHER_HPP_INCLUDED

#include "common.hpp"
#include "storage.hpp"

#include <chrono>

#include <cassert>

using std::literals::chrono_literals::operator""ms;

/**
 * Диспетчер получает координаты объектов в реальном времени
 * и следит за координатами не более чем 10 целей.
 * Цель считается потерянной, если по ней не было координат более 0,5 сек.
 */
class Dispatcher {
	
public:
	// Определим интервал, после которого цель считается потерянной.
	constexpr static const auto LOST_PERIOD = 500ms;
	
	// Максимальное количество одновременно отслеживаемых целей.
	static const size_t MAX_NUMBER_OF_TARGETS = 10;
	
protected:
	// К хранилищу из производных классов будем обращаться по указателю.
	// TODO: По хорошему, надо бы это оформить в виде функции-члена.
	Storage* storage;
	
public:
	// Конструктор связывает диспетчера с хранилищем.
    explicit Dispatcher(Storage* s) : storage(s) {
		assert(storage);
	}
	
	// Виртуальный деструктор позволит корректно уничтожить объект.
	virtual ~Dispatcher() {
	}
	
	// Получение координат объекта от детектора.
	virtual void targetPosition(const Label& id, const Point& pos) = 0;
	
};

#endif  // DISPATCHER_HPP_INCLUDED
