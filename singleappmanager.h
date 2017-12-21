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

#ifndef QTSINGLEAPPMANAGER_H
#define QTSINGLEAPPMANAGER_H

#include <QObject>
#include <QLocalServer>

class SingleAppManager : public QObject
{
    Q_OBJECT
public:
    virtual ~SingleAppManager();

    static SingleAppManager &instance();

    static bool tryRun();

signals:
    void anotherAppStarted() const;

protected:
    QLocalServer m_server;

    bool canRun();

    bool isAnotherRunned();
    bool createServer();

    void checkPing(QLocalSocket *socket);

private:
    static const QLatin1String sc_serverNameTemplate;
    static const QLatin1String sc_pingCommand;

    QString m_serverName;

    explicit SingleAppManager(QObject *parent = 0);
};

#endif // QTSINGLEAPPMANAGER_H
