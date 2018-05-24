/*
 *
 * This file was created in response to CS106B - Assignment 4 - Stanford University
 *
 * Author: Marcus Galdino
 * Date: May 23, 2018
 *
 * Implementation Notes: This file manages the game interacting with the user and the class Boggle that has all the logic for the games.
 * Structure:
 *           BogglePlay.cpp  -> Manages the Game structure: user input, UI, and console
 *           Boggle.cpp      -> Keeps the logic: Sets Board, find words and score players
 *           Boggle.h        -> Defines class Boggle with methods and variables for the whole game
 *           BoggleGUI.cpp   -> User Interface provided by Stanford University
 */

#ifndef _boggle_h
#define _boggle_h

#include <iostream>
#include <string>
#include "lexicon.h"
#include "map.h"
#include "grid.h"

using namespace std;

class Boggle {
public:
    Boggle(Lexicon& dictionary, string boardText = "");

    Grid<string> initBoard(int x,int y,string boardText);
    Grid<bool> resetVisited();

    bool isInLexicon(string input);
    bool prefixLexicon(string input);
    bool isInCube(string input);
    bool testIfVisited();
    bool checkWord(string word);
    bool humanWordSearch(string word);

    void getNeighbors(int k, int l);
    void updatehumanWords(string word);
    void computerTurnAnimation(string prefix);

    char getLetter(int row, int col);
    Set<string> computerWordSearch();
    string setLabels();

    int getScoreHuman();
    int getNumWordsHuman();
    string getWordsHuman();

    int getScoreComputer();
    int getNumWordsComputer();
    string getWordsComputer();

    friend ostream& operator<<(ostream& out, Boggle& boggle);

private:
    Grid<string> board;                   // Keep values for the cubes choosen
    Vector<pair<int,int>> neighbor;       // Saves neighors of a given cube
    string prefix;                        // Test if a given prefix is valid in the lexicon.
    Grid<bool> Visited;                   // Keep values of cubes visited in a given search
    Lexicon dict;                         // Copy of the Lexicon used for testing words in English
    Vector<string> wordsFoundHuman;       // Keep the words human player have found
    Set<string> wordsComputer;            // Keep the words computer player have found
    Vector<string> wordsReceivedHuman;    // Keep all words human player submitted
    int humanscore;                       // Keep human score
    int computerscore;                    // Keep computer score
};



#endif // _boggle_h
