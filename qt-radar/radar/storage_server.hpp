#ifndef STORAGE_SERVER_HPP
#define STORAGE_SERVER_HPP

#include "storage.hpp"

#include <QTcpServer>
#include <QtGlobal>

#include <mutex>

#include <cassert>

/**
 * @brief TCP-сервер для обработки подключений к хранилищу.
 */
class StorageServer : public QTcpServer
{
    Q_OBJECT

private:
    // Хранилище обнаруженных целей.
    Storage* storage;

public:
    // В конструкторе запускам прослушивание порта.
    // Для простоты это порт 9876.
    explicit StorageServer(Storage* s)
        : QTcpServer(), storage(s)
    {
        bool ok = listen(QHostAddress::Any, 9876);
        assert(ok);
    }

protected:
    // Функция-член для получения подключений.
    void incomingConnection(int socketDescriptor) override;

    // Функция-член для обработки одного подключения.
    // Каждое подключение обрабатывается в отдельном потоке.
    void serveConnection(int socketDescriptor);

signals:

public slots:

};

#endif // STORAGE_SERVER_HPP
