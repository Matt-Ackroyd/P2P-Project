#pragma once
#include <QApplication>
#include <qqmlintegration.h>
#include <iostream>
#include <mutex>
#include <QQmlApplicationEngine>
#include "Server.h"
#include "TextChannel.h"
#include "PrimaryClient.h"



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
    Q_INVOKABLE void requestServerInfo(QString Qid);
    Q_INVOKABLE void requestChannelInfo(QObject* server, QObject* channel);
    Q_INVOKABLE void createNewServer();
    Q_INVOKABLE void createNewTextChannel(QString serverid);
    Q_INVOKABLE void joinServer(QString contactAdress, QString contactPort, QString userID, QString inviation); 
    Q_INVOKABLE void createServerInvitation(QString serverid);


    void GUIloadServer(Server* server);
    void GUIloadChannel(TextChannel* channel);
    void GUIloadMessage(MessageContainer* message);
signals:
    void serverLoad(QString signal_param);
    void channelLoad(QString channel_id);
    void messageLoad(QString channel_id, QString message, QString authorName, QString sendDate);
};


