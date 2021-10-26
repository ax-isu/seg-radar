#include "storage_server.hpp"

#include <QTcpSocket>

#include <thread>
#include <sstream>
#include <iostream>
#include <cstring>

#include <cassert>

void StorageServer::incomingConnection(int socketDescriptor) {

    // Для вызова функции-члена в качестве функции потока,
    // используем лямбда-функцию.
    auto threadFunction = [this, socketDescriptor]() {
        this->serveConnection(socketDescriptor);
    };

    // Запускаем поток и отправляем его в свободное плавание.
    std::thread connection(threadFunction);
    connection.detach();

}

void StorageServer::serveConnection(int socketDescriptor) {

    QTcpSocket tcpSocket;
    auto ok = tcpSocket.setSocketDescriptor(socketDescriptor);

    if ( !ok )
        return;

    const size_t BUFFER_SIZE = 256;
    char buffer[BUFFER_SIZE];
    size_t bytes = 0;

    while ( true ) {
        if ( !tcpSocket.waitForReadyRead(200) )
            continue;
        bytes += tcpSocket.readLine(buffer + bytes, BUFFER_SIZE - bytes);
        assert(bytes < BUFFER_SIZE);
        if ( bytes && buffer[bytes - 1] == '\n' ) {
            break;
        }
    }

    std::string since, till;

    std::istringstream in(buffer);
    in >> since >> till;

    std::string json = storage->getObjects(since, till);
    tcpSocket.write(json.c_str());

    tcpSocket.waitForBytesWritten();
}

