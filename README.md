# ESP32_Bitcoin_Ticker

Ein minimalistischer Bitcoin-Preis-Ticker für ESP32 C3 Super Mini und ein WeAct 2.9-Zoll E-Paper Display (3-Farben).

## Features
- Holt Live-Daten von der Binance API.
- Zeigt den aktuellen Preis in EUR.
- Berechnet die 24h-Differenz (Prozent & Absolut).
- Nutzt Deep Sleep (aktualisiert alle 15 Minuten), um Strom zu sparen.

## Hardware
- ESP32 (z.B. DevKit V1)
- Waveshare 2.9" E-Paper Display (BWR)

## Bibliotheken
Folgende Libraries müssen in der Arduino IDE installiert sein:
- `GxEPD2`
- `Adafruit_GFX`


## Anschlussplan (Pinout)

Verbinde das E-Paper Display wie folgt mit deinem ESP32. Die Belegung im Code entspricht diesen Pins:


| E-Paper Pin | ESP32 Pin | Funktion |
| :--- | :--- | :--- |
| **BUSY** | GPIO 21 | Status-Abfrage |
| **CS** | GPIO 10 | Chip Select |
| **RST** | GPIO 6 | Reset |
| **DC** | GPIO 7 | Data/Command |
| **SCK** | GPIO 20 | SPI Clock |
| **MOSI** | GPIO 5 | SPI Data Out |
| **VCC** | 3.3V | Stromversorgung |
| **GND** | GND | Masse |

> **Hinweis:** Da das Display über SPI kommuniziert, achte darauf, dass die Kabelverbindungen fest sitzen, um Grafikfehler zu vermeiden.
