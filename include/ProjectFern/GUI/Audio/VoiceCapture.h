#pragma once
#include <QObject>
#include <QAudioSource>
#include <QAudioBuffer>

//https://stackoverflow.com/a/69256013
class VoiceCapture : public QIODevice
{
   Q_OBJECT
   QAudioFormat m_format;
public:
   VoiceCapture(QObject* parent = {}) : QIODevice(parent) {}
   void start(QAudioSource *source)
   {
      Q_ASSERT(source);
      m_format = source->format();     
      open(QIODevice::WriteOnly);
   }
   qint64 readData(char *, qint64) override { return 0; }
   qint64 writeData(const char *data, qint64 count) override
   {
      //QAudioBuffer buffer({data, static_cast<int>(count)}, m_format);
      emit audioAvailable(data, static_cast<int>(count), m_format);
      return count;
   }
   Q_SIGNAL void audioAvailable(const char *data, int count, QAudioFormat m_format);
};