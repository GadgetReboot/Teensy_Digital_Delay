/* Teensy 3.6 Audio Delay Demo
   Takes audio from memory sample
   Playback is on DAC
   This demo takes the audio and delays it by two different times,
   using the delay tap feature.
   There is no delay feedback so there is only a single playback per delay tap.
   Audio sample taken from freesound.org
   Attribution cited within sketch comments below.

   Gadget Reboot
*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Bounce.h>

// WAV file converted to memory array by wav2sketch
#include "AudioSampleTomtom.h"       // http://www.freesound.org/people/zgump/sounds/86334/

// input pins
#define drumTriggerPin      6        // button to trigger drum sample

// button debounce for trigger input
Bounce button0 = Bounce(drumTriggerPin, 5);  // 5 ms debounce time

// create the audio components
AudioPlayMemory          drumSample;                            // drum sample in memory
AudioOutputAnalog        dac1;                                  // audio out via DAC
AudioMixer4              mixer1;                                // audio mixer
AudioEffectDelay         delay1;                                // delay effect unit

// connect the audio in/out paths with patch cords
AudioConnection          patchCord1(drumSample, 0, mixer1, 0);  // sound sample out to mixer ch0 in
AudioConnection          patchCord2(delay1, 0, mixer1, 1);      // delay tap0 out to mixer ch1 in
AudioConnection          patchCord3(delay1, 1, mixer1, 2);      // delay tap1 out to mixer ch2 in
AudioConnection          patchCord5(drumSample, delay1);        // sound sample out to delay effect in
AudioConnection          patchCord6(mixer1, 0, dac1, 0);        // mixer out to DAC audio input

void setup() {
  // configure the pushbutton pin with pullup
  pinMode(drumTriggerPin, INPUT_PULLUP);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(800);

  // by default the Teensy DAC uses 3.3Vp-p output
  // if your 3.3V power has noise, switching to the
  // internal 1.2V reference can give you a clean signal
  dac1.analogReference(INTERNAL);

  // reduce the gain on mixer channels so more than 1
  // sound can play simultaneously without clipping
  // a level of 0 means the volume is off for that channel
  mixer1.gain(0, 0.5);          // drum sample mixer input
  mixer1.gain(1, 0.5);          // delay unit tap0 mixer input
  mixer1.gain(2, 0.5);          // delay unit tap1 mixer input
  mixer1.gain(3, 0);            // unused input channel

  delay1.delay(0, 400);         // set delay tap ch0 to 400 mS
  delay1.delay(1, 300);         // set delay tap ch1 to 300 mS
  delay1.disable(2);            // disable unused delay tap
  delay1.disable(3);            // disable unused delay tap
  delay1.disable(4);            // disable unused delay tap
  delay1.disable(5);            // disable unused delay tap
  delay1.disable(6);            // disable unused delay tap
  delay1.disable(7);            // disable unused delay tap

  AudioInterrupts();
}

void loop() {

  // update button status
  button0.update();

  // play drum sample if button pressed
  if (button0.fallingEdge()) {
    drumSample.play(AudioSampleTomtom);
  }

}
