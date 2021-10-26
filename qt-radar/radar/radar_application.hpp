#ifndef RADAR_APPLICATION_HPP
#define RADAR_APPLICATION_HPP

#include "detector.hpp"
#include "dispatcher.hpp"
#include "storage.hpp"
#include "storage_server.hpp"

#include <QCoreApplication>

class RadarApplication : public QCoreApplication
{
    Q_OBJECT

private:
    // Хранилище.
    Storage* storage;
    // Диспетчер.
    Dispatcher* dispatcher;
    // Детектор.
    Detector* detector;
    // TCP сервер для хранилища.
    StorageServer* storageServer;

public:
    // Конструктор.
    RadarApplication(int argc, char *argv[]);

    // Деструктор.
    virtual ~RadarApplication();

signals:

public slots:

};

#endif // RADAR_APPLICATION_HPP
