/* Teensy 3.6 Audio Delay Demo
   Takes audio from ADC input or memory sample
   Playback is on DAC
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
#define delayFeedbackPin  A12        // delay feedback level pot
#define delayTimePin      A13        // delay time pot

// button debounce for trigger input
Bounce button0 = Bounce(drumTriggerPin, 5);  // 5 ms debounce time

// delay controls
float delayFbLevel      = 0.1;       // delay feedback level
int delayTime           = 100;       // delay time in mS

// create the audio components
AudioPlayMemory          drumSample;                            // drum sample in memory
AudioInputAnalog         adc1;                                  // audio in via ADC
AudioOutputAnalog        dac1;                                  // audio out via DAC
AudioMixer4              mixer1;                                // audio mixer
AudioEffectDelay         delay1;                                // delay effect unit

// connect the audio in/out paths with patch cords
AudioConnection          patchCord1(delay1, 0, mixer1, 0);      // delay effect out0 to mixer ch0 in
AudioConnection          patchCord2(delay1, 1, mixer1, 1);      // delay effect out1 to mixer ch1 in
AudioConnection          patchCord3(drumSample, 0, mixer1, 2);  // memory sound sample out to mixer ch2 in
AudioConnection          patchCord4(adc1, 0, mixer1, 3);        // adc out to mixer ch3 in
AudioConnection          patchCord5(mixer1, delay1);            // mixer out to delay effect in
AudioConnection          patchCord6(mixer1, 0, dac1, 0);        // mixer out to DAC audio out

void setup() {
  Serial.begin(9600);
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
  mixer1.gain(0, delayFbLevel); // delay unit ch0 mixer input
  mixer1.gain(1, delayFbLevel); // delay unit ch1 mixer input
  mixer1.gain(2, 0.5);          // drum sample mixer input
  mixer1.gain(3, 0.5);            // adc input channel

  // configure delay times for delay tap outputs
  delay1.delay(0, delayTime);   // set delay tap ch0 to delayTime mS
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

  // read delay setting pots
  delayFbLevel = analogRead(delayFeedbackPin);
  delayTime = analogRead(delayTimePin);

  // debug output to verify potentiometer ADC input range
  // and audio memory block usage
  Serial.print("Delay feedback level pot raw reading: ");
  Serial.println(delayFbLevel);
  Serial.print("Delay time pot raw reading: ");
  Serial.println(delayTime);

  // scale raw ADC readings to useful range data
  delayFbLevel = (map(delayFbLevel, 0, 65535, 0, 10) / 10.0);  // mixer level should be between 0 and 1
  Serial.print("Delay feedback level pot scaled: ");
  Serial.println(delayFbLevel);

  delayTime = map(delayTime, 0, 65535, 0, 2000);               // delay between 0 and 2 seconds
  delayTime = round100(delayTime);                             // round by 100 to filter minor ADC fluctuations
  Serial.print("Delay time pot scaled: ");
  Serial.println(delayTime);

  // determine how much actual memory is needed for the audio processes
  Serial.print("Max audio memory blocks used: ");
  Serial.println(AudioMemoryUsageMax());
  Serial.println();

  // apply delay time and level based on pot readings
  delay1.delay(0, delayTime);   // set delay time for delay ch0 to delayTime mS
  mixer1.gain(0, delayFbLevel); // set gain on mixer input for the delay effect

  // update button status
  button0.update();

  // play drum sample if button pressed
  if (button0.fallingEdge()) {
    drumSample.play(AudioSampleTomtom);
  }

}

// round a number off by 100
int round100 (int a)
{
  return  (a / 100 * 100);
}
