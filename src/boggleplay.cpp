// This is a .cpp file you will edit and turn in.
// We have provided a skeleton for you,
// but you must finish it as described in the spec.
// Also remove these comments here and add your own.
// TODO: remove this comment header

// boggleplay.cpp
//boggleplay should not contain any recursion or backtracking;
//all such recursive searching should happen in the Boggle class.

const int BOARD = 16;

#include "lexicon.h"
#include "Boggle.h"
#include "bogglegui.h"
#include "simpio.h"
#include "console.h"
#include <chrono>
#include <thread>

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

void playOneGame(Lexicon& dictionary) {

    string userboard = "";
    string str;
    char ch;


    if (!getYesOrNo("Do you want to generate a random board?(Y/N): ")) {
        while(userboard.length() < BOARD){
            cout << "Enter a Letter from A to Z: " << endl;
            cin >> ch;
            if (isalpha(ch))
                userboard = userboard + ch;
            else
                cout << endl << ch << "is not a valid letter [A-Z]" << endl ;

            if(userboard.length() == BOARD){
                clearConsole();
                cout << "Creating the board game with the letters: " << userboard << endl << endl;
                break;
            }
        }
    }


    clearConsole();

    //Creates Board with user's decicions
    Boggle Board(dictionary,userboard);

    //Starts the User's Gui
    BoggleGUI::initialize(sqrt(BOARD), sqrt(BOARD));
    BoggleGUI::labelAllCubes(Board.setLabels());
    BoggleGUI::setStatusMessage("Ready to Play!");
    BoggleGUI::clearHighlighting();
    BoggleGUI::setAnimationDelay(1);

    cout << endl << "It's your turn!" << endl;

    while(true){

        cout << "Type a word (or Enter 'Q' to stop): " ;
        cin >> str;
        clearConsole();

        std::transform(str.begin(), str.end(), str.begin(), std::ptr_fun<int, int>(std::toupper));

        if (str == "Q"){
            break;
        }

        BoggleGUI::setStatusMessage("Searching for your Word!");
        cout << "Searching for your word: " << str << "..." << endl;
        if (Board.checkWord(str)){
            clearConsole();
            cout << "You found a word!" << endl;
            BoggleGUI::setStatusMessage("You found the word: " + str);
            cout << "Your words: (" << Board.getNumWordsHuman() << ") " << Board.getWordsHuman() << endl << "Your score: (" << Board.getScoreHuman() <<") " << endl;
            BoggleGUI::recordWord(str, BoggleGUI::HUMAN);
            BoggleGUI::setScore(Board.getScoreHuman(), BoggleGUI::HUMAN);

        }
//        else{
//            BoggleGUI::setStatusMessage("Your word could not be found :( ");

//        }
    }

    BoggleGUI::clearHighlighting();

    cout << "it's my turn!" << endl;
    Set<string> result = Board.computerWordSearch();
    cout << "My words: (" << Board.getNumWordsComputer() << ") " << Board.getWordsComputer() << endl << "My score: (" << Board.getScoreComputer() <<") " << endl;

    BoggleGUI::setStatusMessage("Calculating final score...");

    if (Board.getScoreHuman() > Board.getScoreComputer()){
        cout << "Congratulations You beat me!" << endl;
        BoggleGUI::setStatusMessage("Congratulations You beat me!");
    }
    else{
        cout << "Ha ha ha, I destroyed you. Better luck next time, puny human!" << endl;
        BoggleGUI::setStatusMessage("Ha ha ha, I destroyed you. Better luck next time, puny human!");
    }
}

