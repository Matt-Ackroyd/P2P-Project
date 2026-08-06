#include "AudioHandler.h"

#include "VoiceCapture.h"
#include "VoicePlayback.h"


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

    a = new VoiceCapture(this);
    a->start(audioInput);
    audioInput->start(a);


    audioOutput = new QAudioSink(format, this);

    //b = new VoicePlayback(this);
    //b->start(audioInput);
    b = audioOutput->start();

    QObject::connect(a, &VoiceCapture::audioAvailable, this, &AudioHandler::onAudioAvailable);

}

void AudioHandler::onAudioAvailable(const char *data, int count)
{
    this->b->write(data, count);
}



void AudioHandler::onAudioPacketRecived(unsigned char* output)
{
}

void AudioHandler::voiceCaputureCallback(QSpan<float> interleavedAudioBuffer) {
    
}