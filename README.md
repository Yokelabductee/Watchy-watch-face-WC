# Watchy-watch-face-WC
watchy watchface word clock

made with claude code to replicate word clocks, this one is minute accurate whereas most word clocks are 5 min accurate, the first face, font was too small and unreadable along with dithered inactive letters made it unreadable, then I replaced inactive letters with centred dots to still represent the grid format, font for active still unreadable, so I cheated and made font bigger but the letters are still in their correct relative positions to maintain the style and layout of the clock face.

the code also has additional features I added for my own personal use.

I implemented an adjustable(in code) darkmode screen between midnight and 7am the screen is black and does not update to save on battery during sleeping hours.
also hardcoded NTP to time.google server for RTC, included a system to auto update time for BST and GMT so i don't have to manually change at those time changes.
I also disabled the vibration motor in code to also save the battery usage.
