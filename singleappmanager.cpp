/********************************************
**
** MIT License
**
** Copyright (c) 2017 JustCode Justyna Kulinska
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
**
********************************************/

#include "singleappmanager.h"

#include <QLocalSocket>
#include <QCoreApplication>

const QLatin1String SingleAppManager::sc_serverNameTemplate = QLatin1String("%1_SINGLE-APP-SERVER");
const QLatin1String SingleAppManager::sc_pingCommand = QLatin1String("PING!");

SingleAppManager::SingleAppManager(QObject *parent)
    : QObject(parent),
      m_serverName(QString(sc_serverNameTemplate).arg(QCoreApplication::instance()->applicationName()) )
{
    // init connections
    connect(&m_server, &QLocalServer::newConnection, [this]() {
        while (m_server.hasPendingConnections()) {
            auto connection = m_server.nextPendingConnection();

            connect(connection, &QLocalSocket::readyRead,
                    this, [this, connection] () { checkPing(connection); });
        }
    });
}

SingleAppManager::~SingleAppManager()
{}

SingleAppManager &SingleAppManager::instance()
{
    static SingleAppManager instance;
    return instance;
}

bool SingleAppManager::tryRun()
{
    return instance().canRun();
}

bool SingleAppManager::canRun()
{
    if (isAnotherRunned())  // check for windows
        return false;

    if (!createServer())
        return false;

    return true;
}

bool SingleAppManager::isAnotherRunned()
{
    QLocalSocket testSocket;
    testSocket.connectToServer(m_serverName, QLocalSocket::WriteOnly);
    auto connected = testSocket.waitForConnected();
    if (connected) {
        // write ping to another instance
        testSocket.write(sc_pingCommand.data());
        testSocket.waitForBytesWritten();
        testSocket.disconnectFromServer();
        testSocket.waitForDisconnected();   // this line is causing an error on the output, see (QTBUG-50711)
        return true;
    }

    return false;
}

bool SingleAppManager::createServer()
{
    auto success = m_server.listen(m_serverName);
    if (!success &&
            m_server.serverError() == QAbstractSocket::AddressInUseError) {
        QLocalServer::removeServer(m_serverName);

        success = m_server.listen(m_serverName);
    }

    return success;
}

void SingleAppManager::checkPing(QLocalSocket *socket)
{
    Q_ASSERT(socket);
    if (!socket) {
        qWarning() << "Cannot check ping for an null socket instance!";
        return;
    }

    auto readedData = socket->readAll();
    if (readedData.startsWith(sc_pingCommand.data())) {
        emit anotherAppStarted();
    }

    socket->deleteLater();
}
