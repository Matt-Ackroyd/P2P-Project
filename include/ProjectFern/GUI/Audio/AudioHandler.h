#pragma once

#include <QObject>
#include <QAudioSource>
#include <QAudioFormat>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QIODevice>


class AudioHandler : public QObject
{
   Q_OBJECT

public:
    void startVoiceCapture();

private:

    QAudioSource* audioInput;

    void voiceCaputureCallback();
};