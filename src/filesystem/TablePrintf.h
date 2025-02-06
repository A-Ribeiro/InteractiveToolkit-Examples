#pragma once

#include <InteractiveToolkit/InteractiveToolkit.h>

// https://en.wikipedia.org/wiki/ANSI_escape_code

#define INIT_TTY_CMD "\x1B["
#define FINISH_TTY_CMD "m"
#define CMD_CONCAT ";"

#define GEN_RESET "0"
#define GEN_BRIGHT "1"
#define GEN_UNDERSCORE "4"

#define F_BLACK "30"
#define F_RED "31"
#define F_GREEN "32"
#define F_YELLOW "33"
#define F_BLUE "34"
#define F_MAGENTA "35"
#define F_CYAN "36"
#define F_WHITE "37"

#define B_BLACK "40"
#define B_RED "41"
#define B_GREEN "42"
#define B_YELLOW "43"
#define B_BLUE "44"
#define B_MAGENTA "45"
#define B_CYAN "46"
#define B_WHITE "47"

#define COMPOSED_ERROR "\x1B[47;31m" // B comes before F setup

enum class Align : uint8_t
{
    Left,
    Right,
    Center
};

enum class Color : uint8_t
{
    Default,
    Black,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White
};

enum class Weight : uint8_t
{
    Normal,
    Bold
};

class TablePrintf
{

    std::vector<int> size_column;
    int current_column;
    bool should_print;
    bool header;

    void empty_space(int spaces)
    {
        while (spaces > 0)
        {
            putc(' ', stdout);
            spaces--;
        }
    }

public:
    TablePrintf()
    {
        current_column = 0;
        should_print = true;
        header = false;
    }

    void setPrintToOutput(bool v)
    {
        should_print = v;
    }

    void setIsHeader(bool v)
    {
        header = v;
    }

    TablePrintf &addItem(
        const std::string &txt,
        Align align = Align::Left,
        Color foreground = Color::Default,
        Color background = Color::Default,
        Weight weight = Weight::Normal)
    {
        while (size_column.size() <= current_column)
            size_column.push_back(0);
        if (txt.length() > size_column[current_column])
            size_column[current_column] = txt.length();

        if (should_print)
        {
            if (header)
                printf(INIT_TTY_CMD GEN_UNDERSCORE FINISH_TTY_CMD);

            if (current_column > 0)
                printf("|");

            int needed_size = size_column[current_column] - txt.length();
            int needed_size_2 = needed_size >> 1;

            printf(INIT_TTY_CMD);

            if (header) {
                printf(GEN_UNDERSCORE);
                if (weight != Weight::Normal || foreground != Color::Default || background != Color::Default)
                    printf(CMD_CONCAT);
            }

            if (weight == Weight::Bold)
            {
                printf(GEN_BRIGHT);
                if (foreground != Color::Default || background != Color::Default)
                    printf(CMD_CONCAT);
            }

            // foreground
            if (foreground == Color::Black)
                printf(F_BLACK);
            else if (foreground == Color::Red)
                printf(F_RED);
            else if (foreground == Color::Green)
                printf(F_GREEN);
            else if (foreground == Color::Yellow)
                printf(F_YELLOW);
            else if (foreground == Color::Blue)
                printf(F_BLUE);
            else if (foreground == Color::Magenta)
                printf(F_MAGENTA);
            else if (foreground == Color::Cyan)
                printf(F_CYAN);
            else if (foreground == Color::White)
                printf(F_WHITE);

            if (background != Color::Default)
                printf(CMD_CONCAT);

            // background
            if (background == Color::Black)
                printf(B_BLACK);
            else if (background == Color::Red)
                printf(B_RED);
            else if (background == Color::Green)
                printf(B_GREEN);
            else if (background == Color::Yellow)
                printf(B_YELLOW);
            else if (background == Color::Blue)
                printf(B_BLUE);
            else if (background == Color::Magenta)
                printf(B_MAGENTA);
            else if (background == Color::Cyan)
                printf(B_CYAN);
            else if (background == Color::White)
                printf(B_WHITE);

            printf(FINISH_TTY_CMD);

            empty_space(1);

            if (align == Align::Right)
                empty_space(needed_size);
            else if (align == Align::Center)
                empty_space(needed_size_2);

            printf("%s", txt.c_str());

            if (align == Align::Left)
                empty_space(needed_size);
            else if (align == Align::Center)
                empty_space(needed_size - needed_size_2);

            empty_space(1);

            printf(INIT_TTY_CMD GEN_RESET FINISH_TTY_CMD);
        }

        current_column++;

        return *this;
    }

    void endRow()
    {
        if (should_print)
            printf("\n");
        current_column = 0;
    }
};