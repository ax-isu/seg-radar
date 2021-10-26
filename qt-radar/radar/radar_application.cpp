#include "radar_application.hpp"

#include "simple_storage.hpp"
#include "dispatcher_one.hpp"

#include <thread>

// Инициализация приложения Радар.
RadarApplication::RadarApplication(int argc, char *argv[]) : QCoreApplication(argc, argv) {

    // Создадим хранилище, диспетчер, детектор и сервер хранилища.
    storage = new SimpleStorage;
    dispatcher = new DispatcherOne(storage);
    detector = new Detector(dispatcher);
    storageServer = new StorageServer(storage);

    // Запустим детектор в отдельном потоке.
    std::thread thread([](Detector *detector) { detector->start(); }, detector);
    // И позволим ему работать автономно.
    thread.detach();

}

RadarApplication::~RadarApplication() {

    // Удаляем объекты в обратном порядке.
    delete storageServer;
    delete detector;
    delete dispatcher;
    delete storage;

}
