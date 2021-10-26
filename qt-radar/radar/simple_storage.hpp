#ifndef SIMPLE_STORAGE_HPP_INCLUDED
#define SIMPLE_STORAGE_HPP_INCLUDED

// Простейшая реализация хранилища,
// которая хранит только активные цели.

#include "storage.hpp"

#include <thread>
#include <mutex>
#include <regex>
#include <chrono>
#include <ctime>
#include <iostream>

#include <cassert>

/**
 * Хранилище сохраняет данные о координатах целей в течение 6 месяцев
 * и предоставляет интерфейс для их получения внешним системам.
 * Данная реализация не следует этому правилу, а сделана для простоты
 * создания прототипа.
 */
class SimpleStorage : public Storage {
	
	// "Хранить" информацию о целях будем в виде строк JSON.
	typedef std::string JSONTargetRecord;
	
	// Цели храним в ассоциативном массиве.
	std::map<Label, JSONTargetRecord> storage;
	
	// Поскольку доступ к хранилищу может осуществляться параллельно,
	// заведем объект синхронизации для разруливания этого.
	std::mutex criticalSection;
	
	// Шаблон для JSON представления одной записи хранилища.
	const char* jsonTemplate = R"json(
		{
			"id": "@id",
			"state": "@state",
			"ts": "@ts",
			"pos": { x: @x, y: @y }
		}
	)json";
	
public:
	
	// Сохранение данных о цели в хранилище.
	virtual void store(Label id, Point pos, TimeStamp ts, State state) override {
		
		// Сформируем json-запись хранилища. Не очень эффективно, но это пока не важно.
		// Для этого преобразуем в строки состояние, отметку времени и координаты.
		std::string strState =
			state == State::found ? "found" :
			state == State::leading ? "leading" :
			state == State::lost ? "lost" :
			""
		;
		assert(strState != "");
		
		// Чуть сложнее с отметкой времени.
		// TODO: Вынести в отдельную функцию.
		// Сначала с точностью до секунд.
		char strTS[100];
		std::time_t time = std::chrono::system_clock::to_time_t(ts);
		auto sz = std::strftime(strTS, sizeof(strTS), "%d.%m.%Y %H:%M:%S.", std::localtime(&time));
		// Потом миллисекунды.
		using namespace std::chrono;
		time_point<system_clock, milliseconds> ms = time_point_cast<milliseconds>(ts);
		time_point<system_clock, seconds> sec = time_point_cast<seconds>(ts);
		std::sprintf(&strTS[sz], "%03ld", (ms - sec).count());
		
		// Координаты.
		char strX[100], strY[100];
		std::sprintf(strX, "%0.3f", pos.x);
		std::sprintf(strY, "%0.3f", pos.y);
		
		// Собственно, формирование записи JSON.
		std::string jsonRecord = jsonTemplate;
		jsonRecord = std::regex_replace(jsonRecord, std::regex(R"re(\@id)re"), id);
		jsonRecord = std::regex_replace(jsonRecord, std::regex(R"re(\@state)re"), strState);
		jsonRecord = std::regex_replace(jsonRecord, std::regex(R"re(\@ts)re"), strTS);
		jsonRecord = std::regex_replace(jsonRecord, std::regex(R"re(\@x)re"), strX);
		jsonRecord = std::regex_replace(jsonRecord, std::regex(R"re(\@y)re"), strY);
		
		// Выведем в stdout, как требуется в задании.
        std::cout << jsonRecord << std::flush;
		
		// Пока мы сохраняем данные в хранилище (или удаляем)
		// никто не должен иметь к нему доступ. Заблокируем.
		criticalSection.lock();
		
		switch ( state ) {
			
			// Если цель потеряна, удалим её из "хранилища".
			case State::lost:
				storage.erase(id);
				break;
				
			// В остальных случаях добавим цель в "хранилище".
			case State::leading:
			case State::found: {
				storage[id] = jsonRecord;
				break;
			}
			
			default:
				assert(false);
		}
		
		criticalSection.unlock();
		
	}
	
	// Получение данных о целях в хранилище за указанный период в формате JSON.
    virtual std::string getObjects(const std::string& since, const std::string& till) override {
		
		// Просто выдаем, всё, что есть.
        std::string res = "\t[";
		
		// Чтение делаем в критической секции, чтобы никто не внес изменений в это время.
		criticalSection.lock();
		
		auto p = storage.begin();
		
		// Первую запись помещаем без разделителя.
		if ( p != storage.end() ) {
			res += p->second;
			++p;
		}
		
		// Остальные отделяем запятой.
		for ( ; p != storage.end(); ++p ) {
            res += "\t,";
			res += p->second;
		}
		
		criticalSection.unlock();
		
		res += "]\n";
		return res;
		
	}
	
protected:
	
	// Очистка хранилища от устаревших записей.
	virtual void truncate() {
		// Ничего не делаем. Это же прототип.
	}
	
};

#endif // SIMPLE_STORAGE_INCLUDED
