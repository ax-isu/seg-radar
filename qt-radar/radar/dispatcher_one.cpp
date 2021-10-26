#include "dispatcher_one.hpp"

#include <thread>
#include <mutex>
#include <chrono>
#include <set>

// Конструктор диспетчера заполняет пул
// и запускает периодическую проверку потерянных целей.
DispatcherOne::DispatcherOne(Storage* storage)
	: Dispatcher(storage), trackerPool(MAX_NUMBER_OF_TARGETS)
{
	
	// Запускаем периодическую проверку на потерянные цели.
	// Чтобы запустить функцию-член, используем трюк с лямбда-функцией.
	auto start_thread = [this] () { this->handleLostTargets(); };
	std::thread thread(start_thread);
	std::swap(threadForLostTargets, thread);
	
}

// Деструктор останавливает поток проверки на потерянные цели
// и очищает память.
DispatcherOne::~DispatcherOne() {
	
	// Останавливаем поток.
	{
		std::thread thread;
		std::swap(thread, threadForLostTargets);
	}
	
	// Освобождаем память.
	for ( auto kv : activeTrackers ) {
		delete kv.second;
		kv.second = 0;
	}
	
}

// Получение от детектора координат очередного объекта.
// Ищем объект среди активных целей. Если находим, меняем координаты и оповещаем хранилище.
// Если не находим берем из пула новый трекер и делаем то же самое.
// Если пул пуст, игнорируем объект, как если бы он не был обнаружен.
void DispatcherOne::targetPosition(const Label& id, const Point& pos) {
	
	// Запомним время обнаружения объекта.
	auto now = std::chrono::system_clock::now();
	TimeStamp ts = std::chrono::time_point_cast<TimeStamp::duration>(now);
	
	// Чтобы не конфликтовать с функцией handleLostTargets, войдем в критическую секцию.
	criticalSection.lock();
	
	// Поищем объект среди активных целей.
	auto p = activeTrackers.find(id);
	
	// Если нашли, меняем координаты и оповещаем хранилище.
	// Хранилище оповещаем в критической секции, чтобы не зависеть
	// от его реализации (thread safe or not).
	if ( p != activeTrackers.end() ) {
		
		p->second->ts = ts;
		p->second->pos = pos;
		storage->store(id, pos, ts, State::leading);
		
	}
	
	// Если не нашли, но пул не пуст, извлекаем из пула трекер
	// и добавлаем его в список активных трекеров.
	// Хранилище извещаем о том, что обнаружена новая цель.
	if ( p == activeTrackers.end() && !trackerPool.empty() ) {
		
		auto tracker = trackerPool.pop(id, pos, ts);
		activeTrackers.emplace(id, tracker);
		storage->store(id, pos, ts, State::found);
		
	}
	
	// Если же ни то, ни сё, то пропускаем объект, как если бы его и не обнаруживали.
	;
	
	// Выйдем из критической секции.
	criticalSection.unlock();
	
}

// Обработка потерянных целей.
// Это бесконечно работающая функция, которая с некоторым интервалом проверяет,
// есть ли потерянные цели, и, если такие цели есть, удаляет соответствующий
// трекер из списка активных и добавляет его обратно в пул, не забывая оповестить
// хранилище.
void DispatcherOne::handleLostTargets() {
	
	// Определим константу для паузы.
	using namespace std::literals::chrono_literals;
	const auto PAUSE = 200ms;
	
	// Организуем бесконечный цикл, в котором будем определять потерянные цели,
	// обрабатывать их, делать паузу и повторять всё сначала..
	for( ; ; ) {
		
		// Сначала сделаем небольшую паузу.
		std::this_thread::sleep_for(PAUSE);
		
		// Все действия будем выполнять в критической секции,
		// чтобы не конфликтовать с добавлением очередной цели.
		criticalSection.lock();
		
		// Сравнивать будем с текущим временем.
		auto now = std::chrono::system_clock::now();
		
		// Подготовим контейнер для хранения идентификаторов потерянных целей.
		// Целей мало (MAX_NUMBER_OF_TARGETS), поэтому используем любой стандартный контейнер,
		// не сильно задумываясь о производительности по времени и памяти.
		static_assert(MAX_NUMBER_OF_TARGETS < 50);
		std::set<Label> lostTargetIds;
		
		// Пройдемся по активным трекерам и посмотрим, нет ли потерянных целей.
		for ( auto kv : activeTrackers ) {
			if ( kv.second->ts + LOST_PERIOD < now )
				lostTargetIds.insert(kv.first);
		}
		
		// Переместим трекеры потерянных целей из списка активных в пул.
		// Одновременно с этим поместим в хранилище информацию о потерянных целях.
		for ( auto id : lostTargetIds ) {
			auto tracker = activeTrackers[id];
			// В качестве времени потери цели передаем последнее время,
			// когда за целью мы еще следили, а не текущее время.
			storage->store(tracker->id, tracker->pos, tracker->ts, State::lost);
			activeTrackers.erase(id);
			trackerPool.push(tracker);
		}
		
		// Завершим критическую секцию.
		criticalSection.unlock();
		
	}
	
}



