#pragma once
#include <QObject>
#include <QAudioSource>
#include <QAudioBuffer>

class VoicePlayback : public QIODevice
{
    Q_OBJECT
    QAudioFormat m_format;
public:
    VoicePlayback(QObject* parent = {}) : QIODevice(parent) {}
    void start(QAudioSource *source)
    {
        Q_ASSERT(source);
        m_format = source->format();     
        open(QIODevice::ReadWrite);
    }
    qint64 readData(char *, qint64) override 
    { 
        return 1; 
    }
    qint64 writeData(const char *data, qint64 count) override{ return 0; }
};