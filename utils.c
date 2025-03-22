#include <stdio.h>
#include "utils.h"
#include <stdlib.h>

// Helper functions
void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pause_screen() {
    printf("\n按 Enter 键继续...");
    getchar();
}