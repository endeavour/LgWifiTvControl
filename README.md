# LG TV WiFi Controller

This project is designed to expose the RS232 interface on many LG TVs and Projectors over MQTT. This means you can control your TV via your favourite home automation tools.

Instructions:
1) Edit lgcontroller.ino and change the settings at the top
2) Flash onto a NodeMCU device
3) Get a USB-TTL converter such as this (https://www.amazon.co.uk/Raspberry-Programming-Serial-Windows-Supported/dp/B07L61HLV2/ref=sr_1_9?s=electronics&ie=UTF8&qid=1553287387&sr=1-9&keywords=usb+ttl).
 
 Connect white wire to D1
 Connect green wire to D2
 Connect black wire to a ground pin (G).
 Connect red pin to VIn.
 
 Note: If you want to be able to turn on your TV from the off-state, you'll need to power the NodeMCU indepedently rather than via the TV, and also use the mini-jack for serial comms instead of a USB port.
 
 Now you should be able to send commands to the request topic and receive responses on the response topic. The commands can be found in the user manual for your TV.
 

 
 

