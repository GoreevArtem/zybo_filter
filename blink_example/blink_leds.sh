#!/bin/bash

# Экспортируем пины (если еще не экспортированы)
for i in 1019 1020 1021 1022; do
  if [ ! -e /sys/class/gpio/gpio${i} ]; then
    echo $i > /sys/class/gpio/export
  fi
  echo out > /sys/class/gpio/gpio${i}/direction
done

# Основной цикл "переливания"
while true; do
  for i in 1019 1020 1021 1022 1021 1020; do
    # Сначала все гасим
    for j in 1019 1020 1021 1022; do
      echo 0 > /sys/class/gpio/gpio${j}/value
    done
    # Включаем текущий светодиод
    echo 1 > /sys/class/gpio/gpio${i}/value
    sleep 0.1
  done
done
