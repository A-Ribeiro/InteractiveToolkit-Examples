#pragma once

#include <InteractiveToolkit/InteractiveToolkit.h>

void clear_screen()
{
#if defined(_WIN32)
    DWORD Unused = 0;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    DWORD Length = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacterW(GetStdHandle(STD_OUTPUT_HANDLE), ' ', Length, zerozeroc, &Unused);
    FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), csbi.wAttributes, Length, zerozeroc, &Unused);
    COORD top_left = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), top_left);
#elif defined(__APPLE__) || defined(__linux__)
    // CSI[2J clears screen, CSI[H moves the cursor to top-left corner
    printf("\x1B[2J\x1B[H");
    fflush(stdout);
#else

#endif
}

void cursor_to_origin()
{
#if defined(_WIN32)
    COORD top_left = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), top_left);
#elif defined(__APPLE__) || defined(__linux__)
    // CSI[2J clears screen, CSI[H moves the cursor to top-left corner
    //printf("\x1B[2J\x1B[H");
    printf("\x1B[H");
    fflush(stdout);
#else

#endif
}

void sleep_ms(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}