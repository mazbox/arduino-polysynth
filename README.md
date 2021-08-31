# Arduino Polysynth
possibly the cheapest polysynth you can make. It doesn't have an interface - you have to plug in a midi keyboard. It has 10 voices, and can do square and pwm voices, no envelopes or filters or anything like that. It supports pitch bend, sustain pedal and mod wheel controls the PWM. You can choose a pin to output each individual voice or you can just make them all come out of the same pin.

Here's a quick video about it: https://www.youtube.com/watch?v=okLkYKz4yeg

![IMG_1828](https://user-images.githubusercontent.com/194121/131479490-61666c21-1d37-4e0b-a72e-e7feb37f9f10.jpg)

It consists of
* arduino pro micro, 
* 6N138
* 1 diode
* 220 ohm resistor
* 270 ohm resistor
* 47nF capacitor
* MIDI socket
* Audio jack socket

Total cost of parts should be under $10.

Midi circuit is from here: https://www.notesandvolts.com/2012/01/fun-with-arduino-midi-input-basics.html

Here's the complete circuit:

![Untitled_Artwork](https://user-images.githubusercontent.com/194121/131483287-aa9d17da-cc33-4d13-9e17-65edcccb8f31.png)

