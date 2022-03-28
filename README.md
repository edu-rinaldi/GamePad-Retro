# GamePad-Retro

When I was a kid video-games were simple and days 
spent playing with my Nokia 3310 in snake trying to beat 
my own record are countless. So my idea is to pay homage to some of my favorite retro games by creating a tiny retro console.

In particular for this project the following games are 
available: 
- Snake 
- Pong (with its infamous artificial intelligence)


# How it is done
## Arduino components
1. Arduino board (Arduino UNO is fine)
2. Prototyping board
3. OLED Display 0.96" (SSD1306)
4. Passive buzzer
5. IR Transmitter and receiver

![a](/images/Circuit.png)

## External libraries
- ListLib: simple implementation of lists for Arduino [[GitHub source]](https://github.com/luisllamasbinaburo/Arduino-List) (*)
- ezBuzzer: non-blocking library for passive buzzer [[GitHub source]](https://github.com/ArduinoGetStarted/buzzer) (**)
- IRemote: library for decoding IR signals [[GitHub source]](https://github.com/Arduino-IRremote/Arduino-IRremote) 
- u8g2: graphics library for drawing on an SSD1306 OLED display [[GitHub source]](https://github.com/olikraus/u8g2); it provides some drawing primitives like:
    - `drawCircle(…) `
    - `drawBox(…)`
    - …

(*) *Arduino does not come with the STL (Standard Library), so we don’t have lists, queue or other data structure*

(**) *Arduino is a one core board and it does not support multithreading, so using the Tone(…) and NoTone(…) functions in combinations with delay(…), means that we cannot “beep” and in the meanwhile do other stuff in code. This library provides an “escamotage” for achieving this particular result*

More details on **how to import the code and the libraries** in `GamePad.pdf`

# Context
This project was designed for the *"Methods in Computer Science Education: Design"* course at *"Sapienza University of Rome"*. The objective here was not to write reusable/perfect/amazing code, but to build an arduino project to show in high schools with the final objective to get students interested in programming.

However, due to VERY limited memory (thanks arduino uno <3 ) I faced some memory optimization problems.

## Demo

[Here](https://www.youtube.com/watch?v=COxM8bLrkmE) it is a demo of GamePad
