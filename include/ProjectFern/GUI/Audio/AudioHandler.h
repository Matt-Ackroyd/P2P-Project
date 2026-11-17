#pragma once

#include <QObject>
#include <QAudioSource>
#include <QAudioSink>
#include <QAudioFormat>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QIODevice>
#include <QAudioBuffer>

#include "VoiceCapture.h"
#include "VoicePlayback.h"

class AudioHandler : public QObject
{
   Q_OBJECT

public:
    void startVoiceCapture();

    void onAudioPacketRecived(unsigned char *);

    void voiceCaputureCallback(QSpan<float> interleavedAudioBuffer);

private:

    QAudioSource* audioInput;
    QAudioSink* audioOutput;
    VoiceCapture* a;
    QIODevice* b;

private slots:
    void onAudioAvailable(const char *data, int count);

};