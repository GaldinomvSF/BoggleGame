/*
 *
 * This file has not been modified from the original task provided by CS106B Stanford
 *
 * Version: Sat 2014/02/05
 * Author : Marty Stepp
 *
 */

#include <fstream>
#include <iostream>
#include <string>
#include "console.h"
#include "lexicon.h"
#include "random.h"
#include "simpio.h"
#include "strlib.h"
#include "bogglegui.h"
using namespace std;

static const string DICTIONARY_FILE = "dictionary.txt";

void playOneGame(Lexicon& dictionary);

int main() {
    cout << "Welcome to CS 106B Boggle!" << endl;
    cout << "This game is a search for words on a 2-D board of letter cubes." << endl;
    cout << "The good news is that you might improve your vocabulary a bit." << endl;
    cout << "The bad news is that you're probably going to lose miserably to" << endl;
    cout << "this little dictionary-toting hunk of silicon." << endl;
    cout << "If only YOU had a gig of RAM!" << endl;
    cout << endl;
    Lexicon dictionary(DICTIONARY_FILE);
    getLine("Press Enter to begin the game ... ");

    // play games repeatedly until user decides to quit
    while (true) {
        playOneGame(dictionary);   // your function
        cout << endl;
        if (!getYesOrNo("Play again (Y/N)? ")) {
            break;
        }
    }

    cout << "Have a nice day." << endl;
    BoggleGUI::shutdown();
    return 0;
}
