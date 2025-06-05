#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

#define PI 3.14159265358979323846

// Параметры сигнала
#define T 0.00016                // Время в секундах
#define FS 100000000             // Частота дискретизации
#define N ((int)(T * FS))        // Количество отсчётов

// Коэффициенты фильтра
const int COEFFS[] = {
    -255, -260, -312, -288, -144, 153, 616, 1233, 1963, 2739, 3474, 4081, 4481,
    4620, 4481, 4081, 3474, 2739, 1963, 1233, 616, 153, -144, -288, -312, -260, -255
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
            10000 * sin(0.2e6 * 2 * PI * t) +
            1500 * cos(46e6 * 2 * PI * t) +
            2000 * sin(12e6 * 2 * PI * t);

        samples[i] = (int32_t)signal;
    }

    // FIR-фильтрация
    clock_t start = clock();

    for (int i = 0; i < N; i++) {
        long long acc = 0;
        for (int k = 0; k < NUM_COEFFS; k++) {
            if (i - k >= 0) {
                acc += (long long)COEFFS[k] * samples[i - k];
            }
        }
        filtered[i] = (int32_t)(acc / 70000); // Деление на знаменатель, как в lfilter(coeffs, gain)
    }

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
