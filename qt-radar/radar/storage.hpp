#ifndef STORAGE_HPP_INCLUDED
#define STORAGE_HPP_INCLUDED

#include "common.hpp"

#include <string>
#include <chrono>

using std::literals::chrono_literals::operator""h;

/**
 * Хранилище сохраняет данные о координатах целей в течение 6 месяцев
 * и предоставляет интерфейс для их получения внешним системам.
 */
class Storage {

public:
	
	// Период, в течение которого хранится история.
	// Для простоты (чтобы не возиться с календарем)
	// полугодом будем считать 4*31 + 2*30 суток.
	// Независимо от даты отсчета этот период будет не меньше 6 месяцев.
	constexpr static const auto MAX_LIFETIME_OF_TARGET = (4*31 + 2*30) * 24h;
	
	// Сохранение данных о цели в хранилище.
	virtual void store(Label id, Point pos, TimeStamp ts, State state) = 0;
	
	// Получение данных о целях в хранилище за указанный период в формате JSON.
    virtual std::string getObjects(const std::string& since, const std::string& till) = 0;

    // Виртуальный деструктор, чтобы можно было наследовать.
    virtual ~Storage() {
    }

protected:
	// Очистка хранилища от устаревших записей.
	virtual void truncate() = 0;
	
};

#endif // STORAGE_HPP_INCLUDED
