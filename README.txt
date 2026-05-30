WORD CLOCK — Watchy Watch Face
================================

FILES
-----
  WordClock.ino     Main sketch
  WordClockFont.h   DejaVu Sans Mono Bold 9pt, Adafruit GFX format
  settings.h        WiFi credentials + watchySettings struct
  preview.png       200×200 pixel preview of the face

SETUP
-----
1. Edit settings.h — fill in your WiFi SSID and password.
2. Open WordClock.ino in Arduino IDE.
3. Board: "ESP32 Dev Module" (or TTGO T7 v1.4 for Watchy v2+)
   via: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
4. Install the Watchy library via Arduino Library Manager.
5. Enter bootloader: hold Back+Up 5 s → release Back first → release Up.
6. Select the new COM port. Upload.

FEATURES
--------
  • Per-minute word clock  (all 1–29 covered as individual words)
  • MINUTE singular / MINUTES plural  (e.g. "ONE MINUTE PAST TWO")
  • Auto UK BST/GMT  (no code change needed summer→winter)
  • Nighttime blackout  22:00–07:00  (change SLEEP_FROM/SLEEP_UNTIL)
  • Buzzer disabled  (vibrateOClock = false)
  • Battery icon  bottom-right, white on black
  • Black background / white active letters / 50% dithered inactive

FONT
----
  DejaVu Sans Mono Bold, 9 pt, rendered to Adafruit GFX bitmap format.
  Uppercase A–Z + space, centred in 12×16 px cells.
  Regenerate: run the Python script in the repo if you want a different size.
