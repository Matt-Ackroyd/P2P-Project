#include "AudioHandler.h"
#include "VoiceCapture.h"

void AudioHandler::startVoiceCapture() {
    QAudioFormat format;
    // Set up the format, eg.
    format.setSampleRate(44100);
    format.setChannelCount(2);
    format.setSampleFormat(QAudioFormat::Float);

    QAudioDevice info(QMediaDevices::defaultAudioOutput());
    if (!info.isFormatSupported(format)) {
        return;
    }

    audioInput = new QAudioSource(format, this);

    VoiceCapture* a = new VoiceCapture(this);
    a->start(audioInput);
    audioInput->start(a);
}

void AudioHandler::voiceCaputureCallback()
{
}
