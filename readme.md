# LightScheduler 
## Scheduler to automatically control 433Mhz rf remote plugs.
This started as a project for controlling 433Mhz based rc plugs. My 5 plugs and 2 remotes were purchased online. I had no previous knowledge of radio frequency or radio controlled systems and just started tinkering.

**LightScheduler** started on the ArduinoIDE and later moved to **PlatformIO** in **VSCode**. Initially, a nodemcu 8266 wired to a 433Mhz receiver and a male 3.5mm audio plug were used to sniff the various control signals from the origial remote. With the plug inserted to my PC's line in the command signals could be recorded as a sound wave with **Audacity**. A very dirty oscilliscope. Once the comands could be displayed as audio waves. I took screen caps used Photoshop to clean up the images and convert them to binary based on the highs and lows in the wave form. Some how I managed to get one plug working but the others were no-joy. So I decided to ***"read the instructions"***.

After discovering ***#include <RCSwitch.h>*** every thing was much easier. This library has functions to both rx, store and tx 433Mhz rf tristate logic/signals. Thats right, tristate. Are you impressed yet? Perhaps I'll right about it in my memoir in a chapter titled, *"From Zero to Tristate."* Hrumpf! Where were we?

## **Mark I**
The first version was a nodemcu ESP8266, a power LED and 433Mhz tx board. When powered up the it would set variables based on the current elapsed milliseconds using the function *millis();* Using timestamps and durations based on milliseconds it could toggle all plugs on/off every six hours starting at what every moment it was turned on.  

Example:  
Plug it in at 5pm and plugs (lights) turn on and toggle every 6 hours.  
So, on at 5pm, off at 11pm, on at 5am, off at 11am, on at 5pm. Around and around it goes.  
## **Mark II**
Upgraded from the ESP8266 to the ESP32. I developed a single button control code input system using the LED to indicate inputs with blinks. Three control code sequences.  

1. One Press: Turn plugs on/off.  
   Works just like a light switch but for all 5 plugs.  

2. Two Press: Invert the scheduled on/off times.
   Allows the default start time (at power up) to be off not on.  
   
   Plug in at 5am. Normally, this turns on plugs and would be the scheduled on time. With two presses, LED blinks twice to confirm and plugs go off. Now this is the scheduled off time.

3. Three Press: Increases the offset start time.
   The offset value allows the start time to be moved up 5 hours in 1 hour increments. This allow you to set the start time to something other than when it was plugged in.  

   Plug in at 1pm. Plugs turn on and the 1pm,7pm,1am,7am schedule starts. Three presses. LED blinks once. Offset is 1 hour. Now the schedule is 2pm,8pm,2am,8am. Three presses. LED blinks twice. Offset is now 2 hours. Schedule is 3pm,9pm,3am,9am. When offset goes above 5 it resets to 0.

By adding 1-5 hours to the cycle on/off and being able to switch on/off cycles, any on/off schedule could now be set regardless of the power on time.

## **Mark III**
The plan is to add an LCD screen and rotary encoder ***#include <Button2.h>*** and ***#include <ESPRotary.h>*** for controls. Also to switch from elapsed milliseconds to the ***#include <Time.h>*** library so the schedule can be set like any normal clock using hours, minutes and seconds.