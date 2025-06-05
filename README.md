# Проект FIR-фильтрации на платформе Zybo (Zynq-7000)

Этот проект реализует аппаратно-ускоренную FIR-фильтрацию сигналов на плате **Zybo Z7-10** (Xilinx Zynq-7000). Проект сочетает программную обработку на ARM Cortex-A9 с аппаратным ускорением FIR-фильтра в программируемой логике (PL) на основе проектов, список которых представлен ниже. Также есть пример своей реализации работы с фильтром без Pynq на основе обращения к регистрам DMA, которые были взяты из официальной документации Xilinx

Особенности проекта:
- Аппаратный IP-block FIR-фильтра в ПЛИС
- Интеграция с PYNQ (Python environment for Zynq)
- Поддержка DMA для высокоскоростной передачи данных
- Готовый образ Linux с драйверами для аппаратного ускорения

## Сборка на основе референсных проектов

Проект использует следующие референсные реализации:
1. **PYNQ для Zybo**: [nick-petrovsky/PYNQ-ZYBO](https://github.com/nick-petrovsky/PYNQ-ZYBO/tree/main)
2. **Базовый Linux образ**: [Digilent/Zybo-base-linux](https://github.com/Digilent/Zybo-base-linux/tree/master)
3. **Программная и аппаратная реализация Fir фильтра**: [altuSemi/PYNQ4Zybo](https://github.com/altuSemi/PYNQ4Zybo/tree/master)

## Требования

1. **Аппаратные**:
   - Плата Zybo Z7-10
   - SD-карта (16GB+)
   - USB-UART адаптер для отладки

2. **Программные**:
   - Vivado 2022.1+
   - Vitis 2022.1+
   - PetaLinux 2022.1+
   - Python 3.8+ (для PYNQ)

## Инструкция по сборке

### 1. Сборка аппаратного проекта

```bash
cd fpga/
vivado -source scripts/generate_project.tcl
```

После сборки экспортируйте аппаратную платформу (XSA файл) для SDK.

### 2. Сборка образа Linux

На основе [Zybo-base-linux](https://github.com/Digilent/Zybo-base-linux/tree/master):
```bash
git clone https://github.com/Digilent/Zybo-base-linux.git
cd Zybo-base-linux
# Копируем конфигурацию проекта
cp path/to/our/project/configs/zybo_fir_defconfig .config
make
```

### 3. Сборка PYNQ образа

На основе [PYNQ-ZYBO](https://github.com/nick-petrovsky/PYNQ-ZYBO/tree/main):
```bash
git clone https://github.com/nick-petrovsky/PYNQ-ZYBO.git
cd PYNQ-ZYBO
# Копируем наш аппаратный битфайл
cp path/to/our/project/fpga/zybo_filter.bit base/
make
```

### 4. Сборка boot.bin

Используя Xilinx Vitis:
```bash
cd sdk/
vitis -workspace .
# Создаем FSBL проект
# Добавляем аппаратную платформу
# Генерируем boot.bin с компонентами:
#   - FSBL
#   - Bitstream
#   - U-Boot
```

### 5. Подготовка SD-карты

1. Разделы SD-карты:
   - FAT32: boot.bin, image.ub
   - EXT4: корневая файловая система PYNQ

2. Скопируйте файлы:
```bash
cp boot.bin /mnt/boot/
cp image.ub /mnt/boot/
tar xzvf pynq_rootfs.tar.gz -C /mnt/root/
```

## Характеристики FIR-фильтра

Параметры реализации:
- Разрядность данных: 32 бит
- Разрядность коэффициентов: 32 бит
- Тактовая частота: 100 МГц

## Автор

**Артем Гореев**
- GitHub: [@GoreevArtem](https://github.com/GoreevArtem)
- Проект создан в рамках изучения SoC FPGA
