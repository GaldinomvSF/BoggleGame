/*
 *
 * This file was created in response to CS106B - Assignment 4 - Stanford University
 *
 * Author: Marcus Galdino
 * Date: May 23, 2018
 *
 * Implementation Notes: This file manages the game interacting with the user and the class Boggle that has all the logic for the games.
 * Structure:
 *           BogglePlay -> Manages the Game structure: user input, UI, and console
 *           Boggle     -> Keeps the logic: Sets Board, find words and score players
 *
 */


const int BOARD = 16;      //Sets the size of the board to 4x4 -> 16 Cubes;

#include "lexicon.h"
#include "Boggle.h"
#include "bogglegui.h"
#include "simpio.h"
#include "console.h"
#include <chrono>
#include <thread>

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

/* Interacts with the Boggle and BoggleGUI to manage the Game's current state */
void playOneGame(Lexicon& dictionary) {

    string userboard = "";
    string str;
    char ch;

    /**************************************************************************/
    /******** This block receives the User's input for the cubes **************/
    /**************************************************************************/
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
    /******************************* END OF BLOCK ********************************/


    /**************************************************************************/
    /*This block initializes the User's UI with the values chosen by the user */
    /**************************************************************************/

    /* Creates Board with the options:
     * 1) Customized: Userboard variable has the user's input
     * 2) Random: Userboard is an empty string, then computer will generate the cubes */
    Boggle Board(dictionary,userboard);

    //Starts the User's Gui
    BoggleGUI::initialize(sqrt(BOARD), sqrt(BOARD));
    BoggleGUI::labelAllCubes(Board.setLabels());
    BoggleGUI::setStatusMessage("Ready to Play!");
    BoggleGUI::clearHighlighting();
    BoggleGUI::setAnimationDelay(1);

    /******************************* END OF BLOCK ****************************************/

    clearConsole();
    cout << "Ready to Play!" << endl << endl << "Here's the Cube's letters" << endl << Board << endl << "It's your turn!" << endl;

    /**************************************************************************/
    /****** Human's Turn: Loops over the game until the user types 'Q'  *******/
    /**************************************************************************/
    while(true){
        cout << "Type a word (or Enter 'Q' to stop): " ;
        cin >> str;
        clearConsole();
        std::transform(str.begin(), str.end(), str.begin(), std::ptr_fun<int, int>(std::toupper));

        //Quits the game when 'Q' is typed
        if (str == "Q")
            break;

        BoggleGUI::setStatusMessage("Searching for your Word!");
        cout << "Searching for your word: " << str << "..." << endl;

        //Interacts with Boggle class to verify if the word typed is valid
        if (Board.checkWord(str)){
            clearConsole();
            cout << "You found a word!" << endl << endl << "Here's the Cube's letters" << endl << Board << endl ;;
            BoggleGUI::setStatusMessage("You found the word: " + str);
            cout << "Your words: (" << Board.getNumWordsHuman() << ") " << Board.getWordsHuman() << endl << "Your score: (" << Board.getScoreHuman() <<") " << endl;
            BoggleGUI::recordWord(str, BoggleGUI::HUMAN);
            BoggleGUI::setScore(Board.getScoreHuman(), BoggleGUI::HUMAN);
        }
        else{
            cout << endl << "Here's the Cube's letters" << endl << Board << endl ;
        }
    }

    /**************************************************************************/
    /****** Computer's Turn: Prints all words found and final scores  *********/
    /**************************************************************************/

    BoggleGUI::clearHighlighting();
    cout << "It's my Turn!" << endl << endl << Board << endl << "Looking up words" << endl;
    BoggleGUI::setStatusMessage("It's my Turn!");
    Set<string> result = Board.computerWordSearch();
    cout << "My words: (" << Board.getNumWordsComputer() << ") " << Board.getWordsComputer() << endl << "My score: (" << Board.getScoreComputer() <<") " << endl;


    if (Board.getScoreHuman() > Board.getScoreComputer()){
        cout << "Congratulations You beat me!" << endl;
        BoggleGUI::setStatusMessage("Congratulations You beat me!");
    }
    else if(Board.getScoreHuman() < Board.getScoreComputer()){
        cout << "Ha ha ha, I destroyed you. Better luck next time, puny human!" << endl;
        BoggleGUI::setStatusMessage("Ha ha ha, I destroyed you. Better luck next time, puny human!");
    }
    else{
        cout << "Good job, You are as good as me!" << endl;
        BoggleGUI::setStatusMessage("Good Job, You are as good me!");
    }
}

