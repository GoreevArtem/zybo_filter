#!/bin/bash

# Соответствие: кнопка -> светодиод (один к одному)
BTNS=(1011 1012 1013 1014 1015)
LEDS=(1019 1020 1021 1022 1023)

# Экспортируем и настраиваем GPIO
for i in {0..4}; do
  BTN=${BTNS[$i]}
  LED=${LEDS[$i]}
  if [ ! -e /sys/class/gpio/gpio${BTN} ]; then
    echo $BTN > /sys/class/gpio/export
  fi
  if [ ! -e /sys/class/gpio/gpio${LED} ]; then
    echo $LED > /sys/class/gpio/export
  fi
  echo in > /sys/class/gpio/gpio${BTN}/direction
  echo out > /sys/class/gpio/gpio${LED}/direction
done

echo "Жмите кнопки — соответствующие светодиоды будут загораться!"

while true; do
  for i in {0..4}; do
    BTN=${BTNS[$i]}
    LED=${LEDS[$i]}
    val=$(cat /sys/class/gpio/gpio${BTN}/value)
    echo $val > /sys/class/gpio/gpio${LED}/value
  done
  sleep 0.05
done
