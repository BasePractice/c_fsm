// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <common.h>
#include "pneumo_ctrl.h"
/** (-Y1 & -Y2) (Y1) (Y2) (-Y1) (-Y2) (Y1 & Y2) (-Y1) (Y1) (-Y1) */


int main(int argc, char **argv) {
    FILE *fd;
    if (argc < 2) {
        fprintf(stderr, "Не передан файл симуляции\n");
        return EXIT_FAILURE;
    }
    fd = fopen(argv[1], "rt");
    if (0 != fd) {
        struct PneumoEngine engine = {};
        struct String line = {0, 0, 0};
        bool running = true;

        pneumo_engine_init(&engine);
        while (running) {
            int eq_output[2];
            int i;
            char *p;
            if (feof(fd)) {
                fprintf(stdout, "Завершение файла симуляции\n");
                running = false;
                continue;
            }
            string_read_line(fd, &line);
            for (i = 1; i < line.text_len + 1; i += 2) {
                line.text[i] = 0;
            }
            p = &line.text[0];
            engine.cylinders[PNEUMO_CYLINDER_Y1].input_signal[PNEUMO_CYLINDER_SIGNAL_UP] = (int)strtol(p, 0, 10);
            p = &line.text[2];
            engine.cylinders[PNEUMO_CYLINDER_Y1].input_signal[PNEUMO_CYLINDER_SIGNAL_DOWN] = (int)strtol(p, 0, 10);
            p = &line.text[4];
            engine.cylinders[PNEUMO_CYLINDER_Y2].input_signal[PNEUMO_CYLINDER_SIGNAL_UP] = (int)strtol(p, 0, 10);
            p = &line.text[6];
            engine.cylinders[PNEUMO_CYLINDER_Y2].input_signal[PNEUMO_CYLINDER_SIGNAL_DOWN] = (int)strtol(p, 0, 10);
            p = &line.text[8];
            eq_output[PNEUMO_CYLINDER_Y1] = (int)strtol(p, 0, 10);
            p = &line.text[10];
            eq_output[PNEUMO_CYLINDER_Y2] = (int)strtol(p, 0, 10);
            string_reset(&line);
            running = pneumo_engine_tick(&engine);
            if (eq_output[PNEUMO_CYLINDER_Y1] != engine.cylinders[PNEUMO_CYLINDER_Y1].output_signal) {
                fprintf(stderr, "Остановка из-за критической ошибки симуляции автомата. Выходной сигнал первого пневмоцилиндра не совпадает с эталоном\n");
                fflush(stderr);
                running = false;
            } else if (eq_output[PNEUMO_CYLINDER_Y2] != engine.cylinders[PNEUMO_CYLINDER_Y2].output_signal) {
                fprintf(stderr, "Остановка из-за критической ошибки симуляции автомата. Выходной сигнал второго пневмоцилиндра не совпадает с эталоном\n");
                fflush(stderr);
                running = false;
            }
            if (!running) {
                fprintf(stderr, "Остановка из-за критической ошибки автомата\n");
                fflush(stderr);
            }
        }
        string_destroy(&line);
        pneumo_engine_destroy(&engine);
    }
    if (0 != fd) {
        fclose(fd);
    }
    return EXIT_SUCCESS;
}
