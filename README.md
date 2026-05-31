#Oscyloskop STM32 z generatorem DDS AD9850

Projekt z przedmiotu **Programowanie Mikrokontrolerów (PMiK)** — system pomiarowy oparty na STM32F411E-DISCO z funkcją oscyloskopu, wbudowanym generatorem przebiegów AD9850 oraz aplikacją webową do wizualizacji i sterowania.

---

## 📸 Zdjęcia

### Zmontowany układ
<img width="5096" height="3002" alt="IMG_4405" src="https://github.com/user-attachments/assets/9e3f6370-7cf5-49dc-a9a0-27080f277182" />



### Frontend — aplikacja webowa


---

## ✨ Funkcjonalności

- 📊 **Akwizycja sygnału** — ADC 12-bit z transferem DMA (bufor 1024 próbki, tryb circular)
- 〰️ **Generator DDS AD9850** — sinus i prostokąt, zakres 0–40 MHz, sterowanie przez bit-banging
- 📍 **Trigger** — własny moduł z trybami rising/falling/none
- 📺 **Wyświetlacz LCD I2C** — dynamiczny opis stanu pracy (HD44780 + PCF8574)
- 💻 **Frontend Web Serial API** — kontrola częstotliwości, podgląd przebiegu, statystyki Vpp/Min/Max/Avg
- 💾 **Zapis na kartę SD** — eksport próbek do pliku CSV (FatFs + SPI)
- 🔌 **USB CDC (VCP)** — komunikacja dwukierunkowa z aplikacją webową

---

## 🛠️ Wykorzystany sprzęt

| Element | Opis |
|---------|------|
| STM32F411E-DISCO | Mikrokontroler Cortex-M4 96 MHz, 12-bit ADC, USB OTG |
| AD9850 (HC-SR08) | Generator DDS, zegar 125 MHz |
| LCD 16×2 (HD44780) | Wyświetlacz znakowy z konwerterem PCF8574 (I2C) |
| Czytnik kart SD SPI | Moduł czytnika z stabilizatorem 3.3V |

---

## 🔌 Połączenia

```
AD9850 → STM32:
  VCC      → 5V
  GND      → GND
  W_CLK    → PB13
  DATA     → PB15
  FU_UD    → PB1
  RESET    → PB2
  ZOUT2    → PA1 (wejście ADC)

LCD I2C → STM32 (I2C1):
  VCC      → 5V
  GND      → GND
  SDA      → PB9
  SCL      → PB6

Karta SD → STM32 (SPI1):
  VCC      → 5V
  GND      → GND
  MISO     → PA6
  MOSI     → PA7
  SCK      → PA5
  CS       → PB0
```

---

## 📁 Struktura projektu

```
Oscyloskop/
├── Core/
│   ├── Inc/                  # Pliki nagłówkowe
│   │   ├── adc_dma.h         # Moduł ADC + DMA
│   │   ├── trigger.h         # Moduł triggera
│   │   ├── dds_ad9850.h      # Sterownik AD9850
│   │   └── lcd_i2c.h         # Sterownik LCD
│   └── Src/                  # Pliki źródłowe
│       ├── main.c            # Główna pętla aplikacji
│       ├── adc_dma.c
│       ├── trigger.c
│       ├── dds_ad9850.c      # Bit-banging GPIO
│       └── lcd_i2c.c
├── FATFS/
│   └── Target/
│       ├── user_diskio.c     # Integracja z FatFs
│       ├── user_diskio_spi.c # Sterownik SD przez SPI
│       └── user_diskio_spi.h
├── USB_DEVICE/
│   └── App/
│       └── usbd_cdc_if.c     # Odbiór komend z USB
├── frontend/
│   └── oscyloskop.html       # Aplikacja webowa (Web Serial API)
└── README.md
```

---

## 💬 Protokół komend USB CDC

Frontend → STM32:

| Komenda | Działanie |
|---------|-----------|
| `HELLO` | GUI się łączy, LCD przechodzi w tryb pracy |
| `FREQ:1000` | Ustaw częstotliwość 1000 Hz |
| `WAVE:0` / `WAVE:1` | Sinus / Prostokąt |
| `TRIG:R:2048` | Trigger rising edge, próg 2048 |
| `TRIG:F:2048` | Trigger falling edge, próg 2048 |
| `TRIG:N` | Trigger wyłączony |
| `SAVE` | Zapisz próbki ADC do `probki.csv` na karcie SD |
| `BYE` | GUI się rozłącza |

STM32 → Frontend: ciągły strumień próbek ADC (12-bit, little-endian).

---

## 🚀 Uruchomienie

### Wgranie firmware'u
1. Otwórz projekt w STM32CubeIDE
2. Skompiluj (`Project → Build`)
3. Wgraj na płytkę (`Run → Debug` lub `Run → Run`)

### Uruchomienie frontendu
1. Otwórz `frontend/oscyloskop.html` w **Chrome** (wymaga Web Serial API)
2. Kliknij **🔌 Połącz USB** i wybierz port COM ze STM32
3. Steruj częstotliwością i obserwuj przebieg na żywo

---

## 🧠 Najciekawsze wyzwania

- **AD9850** — hardware SPI generował tylko DC, rozwiązanie: bit-banging GPIO
- **LCD I2C** — konflikt pinów I2C1 z układem audio CS43L22, rozwiązanie: PB6/PB9
- **Karta SD** — wymaga karty SDHC sformatowanej na FAT32, SDXC nie zadziała

---

## 👨‍🎓 Autor

**Mateusz Połubiński** 

Jest to rozszerzenie mikrokontrolerowe projektu Arduinoscop, w którym można wykorzystać ten stworzony projekt.
