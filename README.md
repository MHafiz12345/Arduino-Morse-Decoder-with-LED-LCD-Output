# 📡 Morse Code to LCD Translator

![Morse Code Translator Device](https://github.com/user-attachments/assets/2b3238c4-fe5d-494c-97f5-3baf28a7072d)

A bidirectional Morse code translation system built on Arduino that converts Morse code input into readable text on an LCD display and transforms typed text into Morse code signals with visual and audio feedback.

## 📦 Features

- **Input Methods**:
  - Decode manual button presses into Morse code (dot and dash recognition)
  - Convert text entered via Serial Monitor into Morse code signals
  
- **Output Methods**:
  - Real-time character display on 16x2 LCD screen
  - Visual Morse code feedback via LED
  - Auditory Morse code feedback via buzzer/speaker
  
- **Processing Capabilities**:
  - Automatic timing detection for dots, dashes, letter spaces, and word spaces
  - Complete alphanumeric character support (A-Z, 0-9)
  - Support for common punctuation marks and special characters
  - Adjustable speed for Morse code transmission

## 🧰 Hardware Requirements

- Arduino Uno, Nano, or compatible microcontroller
- 16x2 LCD display (with I2C adapter or direct connection)
- Piezo buzzer or small speaker
- Tactile push button for manual input
- LED (for visual Morse output)
- 10kΩ pull-up resistor for button
- 220Ω resistor for LED
- Breadboard and jumper wires

## 📐 Wiring Instructions

| Component | Arduino Connection |
|-----------|-------------------|
| Push Button | Digital Pin 8 & GND (with 10kΩ pull-up to 5V) |
| LED | Digital Pin 13 (with 220Ω resistor) |
| Buzzer/Speaker | Digital Pin 2 |
| LCD Display | Via I2C: SDA, SCL, VCC, GND<br>*or*<br>Direct: see `Adafruit_LiquidCrystal` docs |


## 🔁 How It Works

### Mode 1: Serial Input to Morse Output

1. Connect to the Arduino via Serial Monitor (9600 baud)
2. Type any message in the input field and press Send
3. The system will:
   - Convert each character to its Morse code equivalent
   - Flash the LED and sound the buzzer in the Morse pattern
   - Display the original text on the LCD
   - Print the Morse code representation to Serial Monitor

### Mode 2: Button Input to Text Output

1. Press the button to input Morse code manually:
   - Short press (< 250ms): registers as a dot (•)
   - Long press (≥ 250ms): registers as a dash (−)
2. Release the button to signal the end of a symbol
3. The system will:
   - Interpret pauses between button presses based on timing
   - Decode complete Morse sequences into characters
   - Display decoded characters on the LCD in real-time
   - Print both Morse code and decoded text to Serial Monitor

## ⏱ Morse Timing Reference

The system follows standardized Morse code timing conventions:

| Element | Duration | Representation |
|---------|----------|----------------|
| Dot | `dotLength` | • |
| Dash | `3 × dotLength` | − |
| Symbol space (within letter) | `dotLength` | (no visible representation) |
| Letter space | `3 × dotLength` | / |
| Word space | `7 × dotLength` | // |

### Speed Adjustment

Morse code speed is controlled by the `dotLength` variable:

```cpp
int dotLength = 100; // Default value in milliseconds
```

**Speed calculation**: Words Per Minute (WPM) ≈ 1200 / dotLength

| dotLength (ms) | Approximate WPM |
|----------------|-----------------|
| 50 | 24 |
| 100 | 12 |
| 200 | 6 |


## 🧪 Example Use Cases

- **Educational Tool**: Learn and practice Morse code in an interactive way
- **Communication Aid**: Assist those with speech impairments to communicate via Morse code
- **Emergency Signaling**: Practice emergency communication protocols
- **Hobby Project**: Demonstrate basic principles of digital communication and encoding

## 📝 Morse Code Reference Chart

```
A: .-      N: -.      1: .----    Period: .-.-.-
B: -...    O: ---     2: ..---    Comma: --..--
C: -.-.    P: .--.    3: ...--    Question: ..--..
D: -..     Q: --.-    4: ....-    Slash: -..-.
E: .       R: .-.     5: .....    Equals: -...-
F: ..-.    S: ...     6: -....    Plus: .-.-.
G: --.     T: -       7: --...    Minus: -....
H: ....    U: ..-     8: ---..
I: ..      V: ...-    9: ----.
J: .---    W: .--     0: -----
K: -.-     X: -..-
L: .-..    Y: -.--
M: --      Z: --..
```

