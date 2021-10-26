#ifndef DETECTOR_HPP_INCLUDED
#define DETECTOR_HPP_INCLUDED

#include "dispatcher.hpp"

/**
 * Детектор определяет координаты и идентификаторы объектов и отправляет их диспетчеру.
 * Для каждого отдельно взятого объекта частота обновления координат не превосходит 5 Гц.
 */
class Detector {
	
protected:
	// Доступ к диспетчеру осуществляется через указатель.
	// TODO: Лучше, конечно, через функцию-член...
	Dispatcher* dispatcher;
	
public:
	
	// Инициализация
    explicit Detector(Dispatcher* d) : dispatcher(d) {
	}
	
	// Запуск детектора.
	virtual void start();
	
	// Удаление
	virtual ~Detector() {
	}
	
};

#endif  // DETECTOR_HPP_INCLUDED
