#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

// Параметры сигнала
#define T 0.00016                // Время в секундах
#define FS 100000000             // Частота дискретизации
#define N ((int)(T * FS))        // Количество отсчётов

// Коэффициенты фильтра
const int COEFFS[] = {
    -255, -260, -312, -288, -144, 153, 616, 1233, 1963, 2739,
    3474, 4081, 4481, 4620, 4481, 4081, 3474, 2739, 1963, 1233,
    616, 153, -144, -288, -312, -260, -255
};
const int NUM_COEFFS = sizeof(COEFFS) / sizeof(COEFFS[0]);

int main() {
    // Выделение памяти под массивы
    int32_t* samples = (int32_t*)malloc(N * sizeof(int32_t));
    if (!samples) {
        printf("Memory allocation error\n");
        return 1;
    }

    int32_t* filtered = (int32_t*)calloc(N, sizeof(int32_t));
    if (!filtered) {
        printf("Memory allocation error\n");
        free(samples);
        return 1;
    }

    // Генерация сигнала
    for (int i = 0; i < N; i++) {
        double t = (double)i / FS;

        double signal =
            10000 * sin(0.2e6 * 2 * M_PI * t) +
            1500 * cos(46e6 * 2 * M_PI * t) +
            2000 * sin(12e6 * 2 * M_PI * t);

        samples[i] = (int32_t)signal;
    }

    // Запуск таймера
    clock_t start = clock();

    // === FIR-фильтрация на ARMv8-A ассемблере ===
    for (int i = 0; i < N; i++) {
        register int32_t result;

        __asm__ volatile(
            "mov w5, %w[num_coeffs]\n"          // w5 = NUM_COEFFS (32-бит)
            "mov x6, %[coeffs]\n"               // x6 = адрес коэффициентов
            "mov w7, %w[i]\n"                   // w7 = i (32-бит)
            "mov w8, #0\n"                      // w8 = k (32-бит)
            "mov x9, #0\n"                      // x9 = acc (накопленная сумма, 64-бит)

        "loop_k_asm:\n"
            "cmp w8, w5\n"
            "b.ge end_loop_k_asm\n"

            "sub w10, w7, w8\n"
            "cbnz w10, valid_index_asm\n"       // если i - k >= 0

            "add w8, w8, #1\n"
            "b loop_k_asm\n"

        "valid_index_asm:\n"
            "sxtw x10, w10\n"
            "ldr w11, [%[samples], x10, LSL #2]\n"  // samples[i - k]
            "ldr w12, [x6, w8, SXTW 2]\n"           // coeffs[k]

            "sxtw x11, w11\n"
            "sxtw x12, w12\n"

            "mul x13, x11, x12\n"              // coeff[k] * sample[i - k]
            "adds x9, x9, x13\n"               // acc += ...

            "add w8, w8, #1\n"
            "b loop_k_asm\n"

        "end_loop_k_asm:\n"
            "asr x14, x9, #16\n"               // деление на ~65536 (~= /70000)
            "str w14, [%[filtered], w7, SXTW 2]\n" // filtered[i] = (int32_t)(acc >> 16)

            : /* выходные операнды */
            : /* входные операнды */
              [samples] "r"(samples),
              [filtered] "r"(filtered),
              [coeffs] "r"(COEFFS),
              [i] "r"(i),
              [num_coeffs] "r"((int)NUM_COEFFS)
            : /* изменяемые регистры */
              "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "cc", "memory"
        );
        (void)result;
    }
    // === Конец ассемблерного кода ===

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    // Вывод результатов
    printf("Number of samples: %d\n", N);
    printf("FIR program execution time: %.6f seconds\n", time_spent);

    // Очистка памяти
    free(samples);
    free(filtered);

    return 0;
}