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

#include "Boggle.h"
#include "bogglegui.h"
#include "shuffle.h"
#include "grid.h"
#include <chrono>
#include <thread>

using namespace std::this_thread;   // sleep_for, sleep_until
using namespace std::chrono;        // nanoseconds, system_clock, seconds

const int SIZE = 4;          //Sets line for the Cube 4X4
const int LENGTH = 6;        //Sets number of letters that can be choosen per cube

// letters on all 6 sides of every cube
static string CUBES[16] = {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};


//Creates the board either User Input or Computer Algorithm
Boggle::Boggle(Lexicon& dictionary, string boardText) {

    board = initBoard(SIZE,SIZE,boardText);
    dict = dictionary;
    Visited = resetVisited();
    humanscore = 0;
}

/* Sets up a Grid<string> for the Cube with two options:
* 1) PC: Random Order and letters in the front of the Cubes
* 2) User: Uses the user's input to set up the cubes and letters */
Grid<string> Boggle::initBoard(int x,int y,string boardText){

    Grid<string> temp(SIZE,SIZE);
    Grid<string> newboard(SIZE,SIZE);
    string frontLetter;
    int count = 0;

    if (boardText == ""){
        /* Randonly chooses the order of each Cube on the Board */
        for (int i=0; i !=x ; i++){
            for (int j=0; j != y ;j++){
                temp.set(i,j,CUBES[count]);
                count++;
            }
        }

        shuffle(temp);

        /* Randomly chooses the front letter of each Cube */
        for (int i=0; i !=x ; i++){
            for (int j=0; j != y ;j++){
                frontLetter = temp.get(i,j);
                string shuffled = shuffle(frontLetter);
                newboard.set(i,j,shuffled.substr(1,1));
            }
        }
    }
    else{
        //Sets up the Board based on the User's input

        std::transform(boardText.begin(), boardText.end(), boardText.begin(), std::ptr_fun<int, int>(std::toupper));
        for (int i=0; i !=SIZE ; i++){
            for (int j=0; j != SIZE ;j++){
                newboard.set(i,j,boardText.substr(count,1));
                count++;
            }
        }
    }

    return newboard;
}

/* Find if the word is acceptable:
 * 1) Word have 4+ letters
 * 2) Word exist in the English dictionary
 * 3) word can be formed with the cube  */
bool Boggle::checkWord(string word) {

    //Reset values of previous searches
    Visited = resetVisited();
    neighbor.clear();
    BoggleGUI::clearHighlighting();

    //Make sure that all letters are UpperCase
    std::transform(word.begin(), word.end(), word.begin(), std::ptr_fun<int, int>(std::toupper));

    if (word.length() < 4 ){
        cout << "You have to enter a 4+ letter word from the English Dictionary" << endl;
        BoggleGUI::setStatusMessage("You have to enter a 4+ letter word from the English Dictionary");
        return false;
    }
    else if (!isInLexicon(word)){
        cout << word << " was not found in the Dictonary" << endl ;
        BoggleGUI::setStatusMessage( word + " was not found in the Dictonary" );
        return false;
    }
    else{
        if(!isInCube(word)){
            cout << "Your word was not found in the cube..." << endl ;
            BoggleGUI::setStatusMessage("Your word was not found in the cube...");
        }
        else{
            if(wordsReceivedHuman.contains(word)){
                cout << "You must enter an unfound word ..." << endl ;
                BoggleGUI::setStatusMessage("You must enter an unfound word ...");
            }
            else{
                wordsReceivedHuman.add(word);
                if (humanWordSearch(word)){
                    updatehumanWords(word);
                    clearPastGames();
                    return true;
                }
                else{
                    cout << "Your word can't be formed in the Cube :(" << endl ;
                    BoggleGUI::setStatusMessage("Your word can't be formed in the Cube :( ");
                }
            }
        }
    }

    return false;
}

/* HUMAN's TURN:
 * This function uses Recursive backtracking to form words within the cube based on human input
 * Important Variables:
 *           Localcopy:     Ensures Data Integrity during recursion calls
 *           pairNeighbor:  Keep a pair(row,col) of each neighbor of the cube being searched
 *           Visited:       Keeps track of each cube has been visited on a given search
 *           temp:          Initializes Localcopy on its first execution, all board is considered neighbor
 * Base Case:  All letters could be placed in the board, therefore WORD.LENGTH == 0
 */
bool Boggle::humanWordSearch(string word) {

    Vector<pair<int,int>> Localcopy = neighbor;

    if (word.length() == 0){
        return true;
    }
    else{
        //On First Execution the entire board is consider a "neighbor"
        if (Localcopy.size()==0){
            for (int i =0 ; i < SIZE ; i++){
                for (int j = 0 ; j < SIZE ; j++){
                    pair<int,int> temp;
                    temp.first = i;
                    temp.second = j;
                    Localcopy.add(temp);
                }
            }
        }

        //Goes over all the neighbor of the last letter found in the board
        for (pair<int,int> pairNeighbor : Localcopy){

            //Sets Animation for Backtracking Algorithm
            BoggleGUI::setHighlighted(pairNeighbor.first,pairNeighbor.second,true);
            if (Visited.get(pairNeighbor.first,pairNeighbor.second) == false)
                BoggleGUI::setHighlighted(pairNeighbor.first,pairNeighbor.second,false);

            if (board.get(pairNeighbor.first,pairNeighbor.second) == word.substr(0,1) && Visited.get(pairNeighbor.first,pairNeighbor.second) == false){
                BoggleGUI::setHighlighted(pairNeighbor.first,pairNeighbor.second,true);

                //Chooses the current coordinate(x,y) for this Letter
                Visited.set(pairNeighbor.first,pairNeighbor.second,true);
                neighbor.clear();

                //Find all neighbors for this coordinate(x,y)
                getNeighbors(pairNeighbor.first,pairNeighbor.second);

                //Explore Recursively all coordinates for next letter
                if (humanWordSearch(word.substr(1))){
                    return true;
                }
                else{
                    //Unchoose coordinate (x,y) if the entire word could be formed
                    Visited.set(pairNeighbor.first,pairNeighbor.second,false);
                    neighbor.clear();
                    BoggleGUI::setHighlighted(pairNeighbor.first,pairNeighbor.second,false);
                }
            }
        }

    }
    return false;
}

/* COMPUTER's TURN:
 * Uses Exaustive Backtraking to find all words left by the human player
 * Important Variables:
 *           Localcopy:     Ensures Data Integrity during recursion calls
 *           pairNeighbor:  Keep a pair(row,col) of each neighbor of the cube being searched
 *           Visited:       Keeps track of each cube has been visited on a given search
 *           temp:          Initializes Localcopy on its first execution, all board is considered neighbor
 *           prefix:        Validate if prefix can turn into a real word in english
 * Base Case:  Prefix is an Enlish Word
 */
Set<string> Boggle::computerWordSearch() {

    Set<string> result;

    //Local variable ensures Data Integrity during recursion calls
    Vector<pair<int,int>> Localcopy = neighbor;

    if (dict.contains(prefix) && prefix.length() >= 4 ){
        if(!wordsFoundHuman.contains(prefix)){
            if (!wordsComputer.contains(prefix)){
                result.add(prefix);
                computerTurnAnimation(prefix);
                wordsComputer.add(prefix);
            }
            return result;
        }
    }
    else{
        //On first execution all board is a neighbor
        if (Localcopy.size()==0){
            for (int i =0 ; i < SIZE ; i++){
                for (int j = 0 ; j < SIZE ; j++){
                    pair<int,int> temp;
                    temp.first = i;
                    temp.second = j;
                    Localcopy.add(temp);
                }
            }
        }

        // For each neighbor
        for (pair<int,int> pairNeighbor : Localcopy){
            if(Visited.get(pairNeighbor.first,pairNeighbor.second) == false){

                //Mark current (i,j) as Visited
                Visited.set(pairNeighbor.first,pairNeighbor.second,true);
                BoggleGUI::setHighlighted(pairNeighbor.first,pairNeighbor.second,true);

                string newletter = board.get(pairNeighbor.first,pairNeighbor.second);

                //Find all neighbors for the letter above
                neighbor.clear();
                getNeighbors(pairNeighbor.first,pairNeighbor.second);

                //Add new letter to the current prefix
                string aux = prefix + newletter;

                if (dict.containsPrefix(aux)){
                    prefix = aux;

                    //Recursively explores all neighbors of the prefix formed above
                    result = result + computerWordSearch();

                    prefix = prefix.substr(0,prefix.length()-1);
                    Visited.set(pairNeighbor.first,pairNeighbor.second,false);
                    BoggleGUI::setHighlighted(pairNeighbor.first,pairNeighbor.second,false);

                }
                else{
                    neighbor.clear();
                    Visited.set(pairNeighbor.first,pairNeighbor.second,false);
                    BoggleGUI::setHighlighted(pairNeighbor.first,pairNeighbor.second,false);

                }
            }
        }
    }

    return result;
}

void Boggle::computerTurnAnimation(string prefix){

    computerscore = computerscore + (prefix.length()-3);
    BoggleGUI::setStatusMessage("Computer found a word : " + prefix);
    sleep_for(seconds(2));
    BoggleGUI::recordWord(prefix, BoggleGUI::COMPUTER);
    BoggleGUI::setScore(getScoreComputer(), BoggleGUI::COMPUTER);

}

/* The Following functions test conditions for words the player tried
 * 1) Test if word is on English dictionary
 * 2) Test if prefix is on English dictionary
 * 3) Test if Cube has been visited in this search
 * 4) Test if word exist in the cube (letter by letter)
 */

// 1) Test if word is on English dictionary
bool Boggle::isInLexicon(string input){
    if (dict.contains(input)){
        return true;
    }
    return false;
}

// 2) Test if prefix is on English dictionary
bool Boggle::prefixLexicon(string input){
    if (dict.containsPrefix(input)){
        return true;
    }
    return false;
}

//3) Test if Cube has been visited in this search
bool Boggle::testIfVisited(){
    for (int i=0; i <SIZE ; i++){
        for (int j=0; j < SIZE ;j++){
            if (Visited.get(i,j) == true){
                return true;
            }
        }
    }
    return false;
}

//4) Test if word exist in the cube (letter by letter)
bool Boggle::isInCube(string input) {
    int found = 0;
    string cubevalues = board.toString();

    for (int i = 0 ; i < input.length(); i ++ ){
        for (int j = 0 ; j < cubevalues.length();j++){
            if (input[i] == cubevalues[j]){
                found++;
                break;
            }
        }
    }
    if (found >= input.length())
        return true;
    else
        return false;

}

//Sets Labels on the Cubes
string Boggle::setLabels(){
    string output;
    for (int i=0; i !=SIZE ; i++){
        for (int j=0; j != SIZE ;j++){
            output = output + board.get(i,j);
        }
    }
    return output;
}

//Reset state of the Cubes Visited for a new search
Grid<bool> Boggle::resetVisited(){
    Grid<bool> temp(SIZE,SIZE,false);
    return temp;
}

//Return a letter based on a given coordinate (row,col)
char Boggle::getLetter(int row, int col) {
    string front = board.get(row,col);
    return front[0];
}

/*
 *  This function defines all neighbours of a given cube
 *  It uses variable k & l to establish limnits such as corners in the board
 */
void Boggle::getNeighbors(int i, int j){
    pair<int,int> coordinates;

    for (int k = i-1 ; k <= i+1 ; k++){
        for (int l = j-1; l <= j+1 ; l++){
            if (! (k<0 || l<0 || k>=SIZE || l>=SIZE || (k==i && l==j))){
                coordinates.first = k;
                coordinates.second = l;
                neighbor.add(coordinates);
            }
        }
    }
}

//Resets Variables for a new game
void Boggle::clearPastGames(){
    neighbor.clear();
    Visited = resetVisited();
}


/* Following functions return the current state of the game  */

// 1) Updates score and list of words found by human player
void Boggle::updatehumanWords(string word){
    humanscore = (word.length() - 3) + humanscore;
    wordsFoundHuman.add(word);
}

// 2) Return current human score
int Boggle::getScoreHuman() {
    return humanscore;
}

// 3) Return number of words found by human
int Boggle::getNumWordsHuman() {
    return wordsFoundHuman.size();
}

// 4) Return list of words found by human
string Boggle::getWordsHuman(){
    return wordsFoundHuman.toString();
}

// 5) Return number of words found by Computer
int Boggle::getNumWordsComputer() {
    return wordsComputer.size();
}
// 6) Return the list of words found by Computer
string Boggle::getWordsComputer(){
    return wordsComputer.toString();
}

// 7) Return the current score for the Computer
int Boggle::getScoreComputer() {
    return computerscore;
}

//This function draws a grid for the board on the console
ostream& operator<<(ostream& out, Boggle& boggle) {

    for (int i=0; i !=SIZE ; i++){
        out << "+---+---+---+---+" << endl;
        for (int j=0; j != SIZE ;j++){
            out << "| " << boggle.board.get(i,j) << " " ;
        }
        out << "|" << endl  ;
    }
    out << "+---+---+---+---+" << endl;
    return out;
}
