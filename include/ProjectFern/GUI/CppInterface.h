#pragma once
#include <QApplication>
#include <qqmlintegration.h>
#include <iostream>
#include <mutex>
#include <QQmlApplicationEngine>
#include "Server.h"



class CppInterface : public QObject
{
    Q_OBJECT 
    QML_SINGLETON
    QML_ELEMENT

public:
    explicit CppInterface(QObject *parent = nullptr) : QObject(parent) {}
    // Static pointer to the Singleton instance
    static CppInterface* instancePtr;
    static CppInterface* getInstance();


    Q_INVOKABLE void test();
    Q_INVOKABLE void sendMessage(QString message, QObject* server, QObject* channel);


    void addServer(Server* server);
    void receiveMessage();
signals:
    void serverAdd(QString signal_param);
};


