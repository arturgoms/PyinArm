MCU_SERIES = mk66
MCU = mk66fx1m0

CFLAGS_BOARD = -DF_CPU=180000000

AF_FILE = boards/$(MCU_SERIES)_af.csv
LD_FILE = boards/$(MCU).ld

MICROPY_HW_HAS_SDCARD = 1
