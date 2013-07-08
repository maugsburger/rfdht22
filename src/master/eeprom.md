# EEPROM

Master has at least 2kB of EEPROM, splittet in blocks of 256 bytes here.

With this layout, there are 511 bytes for general usage available, the sensor
and probe data takes 1537 bytes.

Up to 255 sensors can be connected, combined with up to 1024 probes. Every char
in sensor description decrements the available sensor count by one.

## First kB
    0x000…0x01f automatically assigned
        free_sensor_id  first currently unused sensor id
    0x200…0x2ff sensor and probe data start – high section
        0x200: first free probe unit data byte (0x4LL…0x7LL) and length of
                sensor0 description text
        0x201…0x2ff: sensor1-255 probe data start byte (0x4LL…0x7LL) and length of
                sensor1-255 description text
        TTTTTTHH
        TTTTTT  0-64 chars of descriptive text
        HH      0-3 high sector, 0x4LL - 0x7LL
    0x300…0x3ff sensor and probe data start – low section
        0x300: first free probe unit data byte (0xH00…0xHff)
        0x301…0x3ff: sensor1-255 probe data start byte (0xH00…0xHff)

## Second kB
    1024 bytes of "shared" unit data/description string memory
    0x400…0x4ff probe0-255 unit data, according to units.md
    0x500…0x5ff 
    0x600…0x6ff
    0x700…0x7ff

# Examples
## Sensor/Probe Data

Setup with two sensors, first named 'TEST' has 2 probes temp and hum, second
named 'Foo23' has only one temp sensor
   
    free_sensor_id = 2

    0x200   00010000
            TTTTTTHH
            => TTTTTT = 100 => 4 bytes/chars of descriptive text for sensor0
            => HH = 00      => first free high byte is 0x4LL
    0x300   00001100        => first free low byte is  0xH0c

    0x201   00010100
            TTTTTTHH
            => TTTTTT = 101 => 5 bytes/chars of descriptive text for sensor1
            => HH = 00      => data start high byte is 0x4LL
    0x301   00000110        => data start low byte is  0xH06

    0x202…0x2ff --unused--
    0x302…0x3ff --unused--

    0x400   0x54    => 'T'
    0x401   0x60    => 'E'
    0x402   0x53    => 'S'
    0x403   0x54    => 'T'
    0x404   0x40    => °C, divide by 10
    0x405   0x41    => % hum, divide by 10
    0x406   0x46    => 'F'
    0x407   0x6F    => 'o'
    0x408   0x6F    => 'o'
    0x409   0x32    => '2'
    0x40a   0x33    => '3'
    0x40b   0x40    => °C, divide by 10
    0x40c…0x7ff --unused--
