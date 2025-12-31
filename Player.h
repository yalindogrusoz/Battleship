/*
Name: Yalin Dogrusoz
OS: MacOS
Proj: Battleship Final Project
File: Player.h
Purpose: Represents a player (human or bot) in Battleship.
*/

#pragma once
#include <string>
#include "Board.h"
#include "BotAI.h" // for bot behavior
using namespace std;

class Player
{
public:
    enum class Type { Human, Bot };

    Player(const string& name, Type type);

    // setup
    void placeFleet(const vector<int>& sizes, bool manualPlacement);

    //getBoard() overloaded
    Board& getBoard(); // this is non-const to do things like getBoard().placeFleet()
    const Board& getBoard() const; // this is just for printing

    // turn actions
    Position getMove(); // either ask human or use bot AI
    void notifyShotResult(const Position& p, ShotResult result, int sunkIndex);

    // info
    string getName() const;
    bool isBot() const;

private:
    string m_name;
    Type m_type;
    Board m_board;
    BotAI m_ai; // only used if bot
};