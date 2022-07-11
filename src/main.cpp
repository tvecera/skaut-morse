#include <Arduino.h>
#include "AudioTools.h"
#include "AudioLibs/AudioKit.h"
#include "AudioLibs/AudioFaust.h"
#include "noise.h"

#define AI_THINKER_ES8388_VOLUME_HACK 1
#define AUDIOKIT_SETUP_SD 0
#define BUTTON_PIN 18
#define MORSE_FREQUENCY 740
#define VOLUME 60

SineWaveGenerator<int16_t> beep(32000);
GeneratedSoundStream<int16_t> morse(beep);
FaustStream<mydsp> noise;
InputMixer<int16_t> mixer;

AudioKitStream out;
StreamCopy copier(out, mixer);

uint8_t lastState = HIGH;
uint8_t currentState;

void actionKeyDown()
{
    beep.setFrequency(MORSE_FREQUENCY);
    morse.begin();
}

void actionKeyUp()
{
    morse.end();
}

void setup(void)
{
    Serial.begin(115200);
    AudioLogger::instance().begin(Serial, AudioLogger::Warning);

    mixer.add(morse, 1);
    mixer.add(noise, 0.3);

    log_d("Starting I2S...");
    auto kitConfig = out.defaultConfig(TX_MODE);
    kitConfig.sample_rate = 48000;
    kitConfig.channels = 2;
    kitConfig.bits_per_sample = 16;
    out.begin(kitConfig);
    out.setVolume(VOLUME);

    log_d("Starting Faust procesor...");
    auto faustConfig = noise.defaultConfig();
    faustConfig.channels = 1;
    noise.begin(faustConfig);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    log_d("Started...");
}

void loop()
{
    currentState = digitalRead(BUTTON_PIN);

    if (lastState == LOW && currentState == HIGH)
    {
        log_d("Key UP");
        actionKeyUp();
    }

    if (currentState == LOW)
    {
        if (lastState == HIGH)
            log_d("Key DOWN");
        actionKeyDown();
    }

    lastState = currentState;
    copier.copy();
}