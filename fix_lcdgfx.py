from fileinput import FileInput

# Would not compile on ESP8266 without this fix.
with FileInput(".pio/libdeps/d1_mini/lcdgfx/src/lcd_hal/UserSettings.h", inplace=True) as file:
    for line in file:
        print(line.replace("#define CONFIG_ESP32_SPI_ENABLE", "//define CONFIG_ESP32_SPI_ENABLE"), end="")
