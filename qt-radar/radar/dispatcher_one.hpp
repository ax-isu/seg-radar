#ifndef DISPATCHERONE_HPP_INCLUDED
#define DISPATCHERONE_HPP_INCLUDED

#include "dispatcher.hpp"

#include <cassert>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

/**
 * Реализация абстрактного класса Dispatcher.
 */
class DispatcherOne : public Dispatcher {
	
	// Структура хранения информации о текущих координатах цели.
	struct Tracker {
		
		// Координаты цели.
		Point pos;
		
		// Время определения координат.
		TimeStamp ts;
		
		// Идентификатор цели.
		Label id;
		
	};
	
	// Пул трекеров.
	class TrackerPool {
		
		// Контейнер для хранения пула.
		std::vector<Tracker*> container;
		
	public:
		// Заполнение пула десятью трекерами.
		TrackerPool(size_t number_of_targets) {
			container.reserve(number_of_targets);
			for ( size_t i = number_of_targets; i--; )
				container.push_back(new Tracker);
		}
		
		// Очистка пула.
		~TrackerPool() {
			for ( auto& tracker : container ) {
				delete tracker;
				tracker = 0;
			} 
		}
		
	public:
		// Получение трекера с установкой идентифкатора, координат и времени.
		Tracker* pop(Label id, Point pos, TimeStamp ts) {
			
			// Проверка на непустоту контейнера.
			assert(!empty());
			
			// Берем очередной трекер.
			Tracker* tracker = container.back();
			
			// Проверкка на валидность указателя.
			assert(tracker);
			
			// Удаялем трекер из пула.
			container.pop_back();
			
			// Заполняем данные трекера.
			tracker->id = id;
			tracker->pos = pos;
			tracker->ts = ts;
			
			return tracker;
		}
		
	public:
		// Помещение неиспользуемого трекера в пул.
		void push(Tracker* tracker) {
			
			// Трекер должен быть не пуст.
			assert(tracker);
			
			container.push_back(tracker);
		}
		
	public:
		// Проверка пула на пустоту.
		bool empty() const {
			return container.empty();
		}
		
	}; // class DispatcherOne::Tracker
	
	// Список активных трекеров.
	typedef std::map<Label, Tracker*> ActiveTrackers;
	
public:
	// Инициализация.
    explicit DispatcherOne(Storage* storage);
	
	// Деструктор.
	virtual ~DispatcherOne();
	
	// Получение от детектора координат объекта.
	virtual void targetPosition(const Label& id, const Point& pos) override;
	
private:
	// Определение потерянных целей.
	void handleLostTargets();
	
private:
	// Пул трекеров.
	TrackerPool trackerPool;
	
	// Список активных трекеров.
	ActiveTrackers activeTrackers;
	
	// Нам еще потребуется критическая секция.
	std::mutex criticalSection;
	
	// А также переменная для хранения потока, выявляющего "потерянные" цели.
	std::thread threadForLostTargets;
	
}; // class DispatcherOne

#endif  // DISPATCHERONE_HPP_INCLUDED
