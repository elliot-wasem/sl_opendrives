/*========================================
 *    sl.c: SL version 5.03
 *        Copyright 1993,1998,2014-2015
 *                  Toyoda Masashi
 *                  (mtoyoda@acm.org)
 *        Last Modified: 2014/06/03
 *========================================
 */
/* sl version 5.03 : Fix some more compiler warnings.                        */
/*                                              by Ryan Jacobs    2015/01/19 */
/* sl version 5.02 : Fix compiler warnings.                                  */
/*                                              by Jeff Schwab    2014/06/03 */
/* sl version 5.01 : removed cursor and handling of IO                       */
/*                                              by Chris Seymour  2014/01/03 */
/* sl version 5.00 : add -c option                                           */
/*                                              by Toyoda Masashi 2013/05/05 */
/* sl version 4.00 : add C51, usleep(40000)                                  */
/*                                              by Toyoda Masashi 2002/12/31 */
/* sl version 3.03 : add usleep(20000)                                       */
/*                                              by Toyoda Masashi 1998/07/22 */
/* sl version 3.02 : D51 flies! Change options.                              */
/*                                              by Toyoda Masashi 1993/01/19 */
/* sl version 3.01 : Wheel turns smoother                                    */
/*                                              by Toyoda Masashi 1992/12/25 */
/* sl version 3.00 : Add d(D51) option                                       */
/*                                              by Toyoda Masashi 1992/12/24 */
/* sl version 2.02 : Bug fixed.(dust remains in screen)                      */
/*                                              by Toyoda Masashi 1992/12/17 */
/* sl version 2.01 : Smoke run and disappear.                                */
/*                   Change '-a' to accident option.                         */
/*                                              by Toyoda Masashi 1992/12/16 */
/* sl version 2.00 : Add a(all),l(long),F(Fly!) options.                     */
/*                                              by Toyoda Masashi 1992/12/15 */
/* sl version 1.02 : Add turning wheel.                                      */
/*                                              by Toyoda Masashi 1992/12/14 */
/* sl version 1.01 : Add more complex smoke.                                 */
/*                                              by Toyoda Masashi 1992/12/14 */
/* sl version 1.00 : SL runs vomiting out smoke.                             */
/*                                              by Toyoda Masashi 1992/12/11 */

#include <curses.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "sl.h"

void add_smoke(int y, int x);
void add_man(int y, int x);
int add_C51(int x);
int add_D51(int x);
int add_sl(int x);
int add_horses(int x);
void option(int argc, char *argv[]);
int isnumber(char *n);
int my_mvaddstr(int y, int x, char *str);

config conf = {.accident = 0, .selection = ERR, .logo = 0, .fly = 0, .c51 = 0};

/**
 * Adds a string safely to the screen buffer.
 *
 * @param y, x - y and x coordinates on screen
 * @param str - string to print to screen buffer
 * 
 * @returns OK(0) if all went well, or ERR(-1) if an error occurred
 */
int my_mvaddstr(int y, int x, char *str)
{
    // Iterates across string, effectively truncating anything before index 0.
    // This allows the caller to print things that hang off the left of the screen.
    for ( ; x < 0; ++x, ++str)
        if (*str == '\0')  return ERR;
    // prints the string to buffer
    for ( ; *str != '\0'; ++str, ++x)
        if (mvaddch(y, x, *str) == ERR)  return ERR;
    return OK;
}

/**
 * Returns OK(0) if string represents a number, and ERR(-1) otherwise.
 * 
 * @param n - string to check
 * 
 * @returns OK(0) if the string is a number, and ERR(-1) otherwise.
 */
int isnumber(char *n) {
    for (int i = 0; n[i] != '\0'; i++) {
        if (!isdigit(n[i])) {
            return ERR;
        }
    }
    return OK;
}

/**
 * Collects options from argv idiomatically using getopt
 */
void option(int argc, char *argv[])
{
    extern config conf;
    int opt;
    while ((opt = getopt(argc, argv, "aFlcn:")) != -1) {
        switch (opt) {
            case 'a': conf.accident = 1; break;
            case 'F': conf.fly       = 1; break;
            case 'l': conf.logo      = 1; break;
            case 'c': conf.c51       = 1; break;
            case 'n': 
                if (isnumber(optarg) == OK) {
                    conf.selection = atoi(optarg);
                }
                break;
        }
    }
}

int main(int argc, char *argv[])
{
    extern config conf;
    int x;

    option(argc, argv);

    initscr();
    // TODO: uncomment line below
    // signal(SIGINT, SIG_IGN);

    // don't echo typed characters to screen
    noecho();
    // cursor invisible
    curs_set(0);
    // leaves the cursor wherever it last was,
    // reduces time spent moving cursor around
    leaveok(stdscr, TRUE);
    // disables scrolling even if the cursor goes off the bottom of the screen
    scrollok(stdscr, FALSE);

    int print_selection_error = 0;

    if (conf.selection == ERR) {
        srand(time(NULL));
        conf.selection = rand() % 4;
    }

    // draws selected train across screen
    for (x = COLS - 1; ; --x) {
        if (conf.logo == 1) {
            if (add_sl(x) == ERR) break;
        }
        else if (conf.c51 == 1) {
            if (add_C51(x) == ERR) break;
        }
        else {
            int error_occurred = 0;
            switch (conf.selection) {
                case 0:
                    if (add_sl(x) == ERR) error_occurred = 1;
                    break;
                case 1:
                    if (add_C51(x) == ERR) error_occurred = 1;
                    break;
                case 2:
                    if (add_D51(x) == ERR) error_occurred = 1;
                    break;
                case 3:
                    if (add_horses(x) == ERR) error_occurred = 1;
                    break;
                default:
                    print_selection_error = 1;
                    error_occurred = 1;
                    break;
            }
            if (error_occurred) {
                break;
            }
        }
        refresh();
        usleep(40000);
    }
    mvcur(0, COLS - 1, LINES - 1, 0);
    endwin();

    if (print_selection_error) {
        fprintf(stderr, "ERROR: Locomotive selection '%d' is invalid\n", conf.selection);
    }

    return 0;
}

/**
 * Draws little train, centering it vertically
 *
 * @param x - horizontal position of train
 * 
 * @return ERR(-1) if parameter x is less than -LOGOLENGTH,
 *             and otherwise OK(0)
 */
int add_sl(int x)
{
    extern config conf;
    // 6 stages of the train
    static char *sl[LOGOPATTERNS][LOGOHEIGHT + 1]
        = {{LOGO1, LOGO2, LOGO3, LOGO4, LWHL11, LWHL12, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL21, LWHL22, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL31, LWHL32, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL41, LWHL42, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL51, LWHL52, DELLN},
           {LOGO1, LOGO2, LOGO3, LOGO4, LWHL61, LWHL62, DELLN}};

    static char *coal[LOGOHEIGHT + 1]
        = {LCOAL1, LCOAL2, LCOAL3, LCOAL4, LCOAL5, LCOAL6, DELLN};

    static char *car[LOGOHEIGHT + 1]
        = {LCAR1, LCAR2, LCAR3, LCAR4, LCAR5, LCAR6, DELLN};

    // py1, py2, py3 positions of coal car and 2 passenger cars
    int i, y, py1 = 0, py2 = 0, py3 = 0;

    if (x < - LOGOLENGTH)  return ERR;
    
    // calculates vertical positioning
    y = LINES / 2 - 3;

    if (conf.fly == 1) {
        y = (x / 6) + LINES - (COLS / 6) - LOGOHEIGHT;
        py1 = 2;  py2 = 4;  py3 = 6;
    }
    
    // iterate over each line of the train and draw it
    for (i = 0; i <= LOGOHEIGHT; ++i) {
        // draws locomotive
        my_mvaddstr(y + i, x, sl[(LOGOLENGTH + x) / 3 % LOGOPATTERNS][i]);
        // draws coal car
        my_mvaddstr(y + i + py1, x + 21, coal[i]);
        // draws passenger car 1
        my_mvaddstr(y + i + py2, x + 42, car[i]);
        // draws passenger car 2
        my_mvaddstr(y + i + py3, x + 63, car[i]);
    }

    // if accident flag has been set, add people
    if (conf.accident == 1) {
        add_man(y + 1, x + 14);
        add_man(y + 1 + py2, x + 45);  add_man(y + 1 + py2, x + 53);
        add_man(y + 1 + py3, x + 66);  add_man(y + 1 + py3, x + 74);
    }
    add_smoke(y - 1, x + LOGOFUNNEL);
    return OK;
}


int add_D51(int x)
{
    extern config conf;
    // 6 stages of the train, 10 lines high
    static char *d51[D51PATTERNS][D51HEIGHT + 1]
        = {{D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL11, D51WHL12, D51WHL13, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL21, D51WHL22, D51WHL23, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL31, D51WHL32, D51WHL33, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL41, D51WHL42, D51WHL43, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL51, D51WHL52, D51WHL53, D51DEL},
           {D51STR1, D51STR2, D51STR3, D51STR4, D51STR5, D51STR6, D51STR7,
            D51WHL61, D51WHL62, D51WHL63, D51DEL}};
    static char *coal[D51HEIGHT + 1]
        = {COAL01, COAL02, COAL03, COAL04, COAL05,
           COAL06, COAL07, COAL08, COAL09, COAL10, COALDEL};

    int y, i, dy = 0;

    if (x < - D51LENGTH)  return ERR;

    // calculates vertical positioning
    y = LINES / 2 - 5;

    if (conf.fly == 1) {
        y = (x / 7) + LINES - (COLS / 7) - D51HEIGHT;
        dy = 1;
    }
    // print locomotive and coal car 1 line at a time
    for (i = 0; i <= D51HEIGHT; ++i) {
        my_mvaddstr(y + i, x, d51[(D51LENGTH + x) % D51PATTERNS][i]);
        my_mvaddstr(y + i + dy, x + 53, coal[i]);
    }
    // if accident, print guys
    if (conf.accident == 1) {
        add_man(y + 2, x + 43);
        add_man(y + 2, x + 47);
    }
    add_smoke(y - 1, x + D51FUNNEL);
    return OK;
}

int add_C51(int x)
{
    extern config conf;
    static char *c51[C51PATTERNS][C51HEIGHT + 1]
        = {{C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH11, C51WH12, C51WH13, C51WH14, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH21, C51WH22, C51WH23, C51WH24, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH31, C51WH32, C51WH33, C51WH34, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH41, C51WH42, C51WH43, C51WH44, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH51, C51WH52, C51WH53, C51WH54, C51DEL},
           {C51STR1, C51STR2, C51STR3, C51STR4, C51STR5, C51STR6, C51STR7,
            C51WH61, C51WH62, C51WH63, C51WH64, C51DEL}};
    static char *coal[C51HEIGHT + 1]
        = {COALDEL, COAL01, COAL02, COAL03, COAL04, COAL05,
           COAL06, COAL07, COAL08, COAL09, COAL10, COALDEL};

    int y, i, dy = 0;

    if (x < - C51LENGTH)  return ERR;
    y = LINES / 2 - 5;

    if (conf.fly == 1) {
        y = (x / 7) + LINES - (COLS / 7) - C51HEIGHT;
        dy = 1;
    }
    for (i = 0; i <= C51HEIGHT; ++i) {
        my_mvaddstr(y + i, x, c51[(C51LENGTH + x) % C51PATTERNS][i]);
        my_mvaddstr(y + i + dy, x + 55, coal[i]);
    }
    if (conf.accident == 1) {
        add_man(y + 3, x + 45);
        add_man(y + 3, x + 49);
    }
    add_smoke(y - 1, x + C51FUNNEL);
    return OK;
}


void add_man(int y, int x)
{
    static char *man[2][2] = {{"", "(O)"}, {"Help!", "\\O/"}};
    int i;

    for (i = 0; i < 2; ++i) {
        my_mvaddstr(y + i, x, man[(LOGOLENGTH + x) / 12 % 2][i]);
    }
}


void add_smoke(int y, int x)
#define SMOKEPTNS        16
{
    static struct smokes {
        int y, x;
        int ptrn, kind;
    } S[1000];
    static int sum = 0;
    static char *Smoke[2][SMOKEPTNS]
        = {{"(   )", "(    )", "(    )", "(   )", "(  )",
            "(  )" , "( )"   , "( )"   , "()"   , "()"  ,
            "O"    , "O"     , "O"     , "O"    , "O"   ,
            " "                                          },
           {"(@@@)", "(@@@@)", "(@@@@)", "(@@@)", "(@@)",
            "(@@)" , "(@)"   , "(@)"   , "@@"   , "@@"  ,
            "@"    , "@"     , "@"     , "@"    , "@"   ,
            " "                                          }};
    static char *Eraser[SMOKEPTNS]
        =  {"     ", "      ", "      ", "     ", "    ",
            "    " , "   "   , "   "   , "  "   , "  "  ,
            " "    , " "     , " "     , " "    , " "   ,
            " "                                          };
    static int dy[SMOKEPTNS] = { 2,  1, 1, 1, 0, 0, 0, 0, 0, 0,
                                 0,  0, 0, 0, 0, 0             };
    static int dx[SMOKEPTNS] = {-2, -1, 0, 1, 1, 1, 1, 1, 2, 2,
                                 2,  2, 2, 3, 3, 3             };
    int i;

    if (x % 4 == 0) {
        for (i = 0; i < sum; ++i) {
            my_mvaddstr(S[i].y, S[i].x, Eraser[S[i].ptrn]);
            S[i].y    -= dy[S[i].ptrn];
            S[i].x    += dx[S[i].ptrn];
            S[i].ptrn += (S[i].ptrn < SMOKEPTNS - 1) ? 1 : 0;
            my_mvaddstr(S[i].y, S[i].x, Smoke[S[i].kind][S[i].ptrn]);
        }
        my_mvaddstr(y, x, Smoke[sum % 2][0]);
        S[sum].y = y;    S[sum].x = x;
        S[sum].ptrn = 0; S[sum].kind = sum % 2;
        sum ++;
    }
}

int add_horses(int x)
{
    extern config conf;
    // 3 stages of the horses, 6 lines high
    static char *horses[HORSESPATTERNS][HORSESHEIGHT+1]
        = {
            {HORSES11, HORSES12, HORSES13, HORSES14, HORSES15, HORSES16, HORSES17, HORSES18},
            {HORSES21, HORSES22, HORSES23, HORSES24, HORSES25, HORSES26, HORSES27, HORSES28},
            {HORSES31, HORSES32, HORSES33, HORSES34, HORSES35, HORSES36, HORSES37, HORSES38},
        };

    int y, i, dy = 0;

    if (x < - HORSESLENGTH)  return ERR;

    // calculates vertical positioning
    y = LINES / 2 - (HORSESHEIGHT/2);

    if (conf.fly == 1) {
        y = (x / 7) + LINES - (COLS / 7) - HORSESHEIGHT;
        dy = 2;
    }
    // print locomotive and coal car 1 line at a time
    for (i = 0; i < HORSESHEIGHT; ++i) {
        my_mvaddstr(y + i,           x,      horses[(HORSESLENGTH + x)     % HORSESPATTERNS][i]);
        my_mvaddstr(y + i + dy,      x + 18, horses[(HORSESLENGTH + x + 1) % HORSESPATTERNS][i]);
        my_mvaddstr(y + i + dy + dy, x + 36, horses[(HORSESLENGTH + x + 2) % HORSESPATTERNS][i]);
    }
    // if accident, print guys
    if (conf.accident == 1) {
        //add_man(y + 2, x + 43);
        //add_man(y + 2, x + 47);
    }
    return OK;
}