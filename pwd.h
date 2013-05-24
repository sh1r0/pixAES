#ifndef PWD_H
#define PWD_H

#include <cstdio>
#include <iostream>

using namespace std;

#define MAX_PWD_LEN 20

#ifdef _WIN32
#include <conio.h>
#define EOL '\r'
#else
#include <termios.h>
#include <unistd.h>
#define EOL '\n'

int getch(void)
{
    int ch;
    struct termios oldt, newt;

    // get terminal input's attribute
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    //set termios' local mode
    newt.c_lflag &= ~(ECHO|ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    //read character from terminal input
    ch = getchar();

    //recover terminal's attribute
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}

#endif // _WIN32

string getPassword()
{
    char *pwd = new char[MAX_PWD_LEN+1];
    unsigned char c;
    int i = 0;

    while ((c=getch())!=EOL) {
        if (i < MAX_PWD_LEN && isprint(c)) {
            pwd[i++] = c;
            putchar('*');
        }
        else if (i > 0 && (c == '\b' || c == 0x7F)) {
            i--;
            putchar('\b');
            putchar(' ');
            putchar('\b');
        }
    }
    putchar('\n');
    pwd[i] = '\0';

    return string(pwd);
}

#endif
