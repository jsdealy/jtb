#pragma once
#include <vector>
#include <ncurses.h>
#include <string>


class Display {
    int numberOfPersistentLines;
    int numberOfDisplayMessages;
public:
    Display(std::vector<std::string> persistentLines): numberOfPersistentLines(persistentLines.size()), numberOfDisplayMessages(0) {
	// Initialize ncurses
	initscr();
	cbreak();      // Line buffering disabled, pass everything to me
	noecho();      // Don't echo() while we do getch
	keypad(stdscr, TRUE);  // Enable special keys
	for (std::string s : persistentLines) {
	    printw("%s", s.c_str());
	}
	refresh();
    }

    Display(): numberOfPersistentLines(0), numberOfDisplayMessages(0) {
	// Initialize ncurses
	initscr();
	cbreak();      // Line buffering disabled, pass everything to me
	noecho();      // Don't echo() while we do getch
	keypad(stdscr, TRUE);  // Enable special keys
	refresh();
    }

    ~Display() {
	endwin();
    }

    void flashMessage(const char* s, int milliseconds = 250) {
	printw("%s", s);
	refresh();
	/* sleep for milliseconds <= 01/13/24 13:27:21 */ 
	napms(milliseconds);
	/* clearing the flash messages <== 12/07/24 14:32:19 */ 
	move(numberOfPersistentLines + numberOfDisplayMessages,0);
	clrtobot();
	refresh();
    }

    void displayMessage(const char* s) {
	++numberOfDisplayMessages;
	printw("%s", s);
	refresh();
    }

    void clearMessages(int wait = 0, bool debug = false) {
	move(numberOfPersistentLines, 0);
	if (debug) {
	    printw("%s", "About to clear messages...");
	    refresh();
	}
	napms(wait);
	clrtobot();
	refresh();
	numberOfDisplayMessages = 0;
    }
};
