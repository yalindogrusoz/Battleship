/*
Name: Yalin Dogrusoz
OS: MacOS
Proj: Battleship Final Project
File: main.cpp
Purpose: Player vs Bot game loop (colored boards, classic or russian fleets).
*/

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include "Board.h"
#include "Player.h"
#include "Types.h"
#include "Position.h"
using namespace std;

static void pickFleet(vector<int>& sizes)
{
    cout << "Choose mode: classic (c) or russian (r): ";
    char m; 
    cin >> m;
    if (m == 'r')
    {
        // Russian
        sizes = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    }
    else
    {
        // Classic
        sizes = {5, 4, 3, 3, 2};
    }
}

static bool inBoundsPos(const Position& p)
{
    return ((p.row >= 0 && p.row < 10) && (p.col >= 0 && p.col < 10));
}

int main()
{
    srand(time(0));
    cout << "Welcome to Battleship! (You vs Bot)\n";

    //  Pick fleet
    vector<int> sizes;
    pickFleet(sizes);

    // Create players
    Player human("You", Player::Type::Human);
    Player bot("Bot", Player::Type::Bot);

    // Human placement choice
    cout << "Place ships manually (m) or randomize (r)? ";
    char placeChoice;
    cin >> placeChoice;
    bool manual = (placeChoice == 'm');

    // Place fleets
    human.placeFleet(sizes, manual);
    bot.placeFleet(sizes, false); // bot always random

    // Track human shots so we don’t repeat the same cell
    bool humanFired[10][10] = {false};

    bool gameOver = false;
    bool playerTurn = true;

    while (!gameOver)
    {
        if (playerTurn)
        {
            cout << "\nYour turn:\n";
            cout << "(Your board)\n";
            human.getBoard().printForOwner();
            cout << "\n(Enemy board — ships hidden)\n";
            bot.getBoard().printForOpponent();

            // Ask until we get a valid, not-yet-fired target
            Position t;
            while (true)
            {
                cout << "Enter target (e.g., B7): ";
                string s;
                cin >> s;
                if (s.size() < 2) continue;

                t.col = toupper(s[0]) - 'A';
                // parse row
                int rowNum = 0;
                try { rowNum = stoi(s.substr(1)); }
                catch (...) { rowNum = 0; }
                t.row = rowNum - 1;

                if (!inBoundsPos(t)) { cout << "Out of bounds. Try again.\n"; continue; }
                if (humanFired[t.row][t.col]) { cout << "You already fired there. Try again.\n"; continue; }
                break;
            }
            humanFired[t.row][t.col] = true; // update the position to show that that cell has been fired before

            int sunkIdx = -1;
            ShotResult res = bot.getBoard().fireAt(t, sunkIdx);

            if (res == ShotResult::Hit)
            {
                cout << "Hit!\n";
            }
            else if (res == ShotResult::Sunk)
            {
                cout << "You sunk a ship!\n";
                bot.getBoard().revealPerimeterOf(sunkIdx);
            }
            else
            {
                cout << "Miss.\n";
            }

            // Human doesn't need AI updates; bot does on its turn

            if (bot.getBoard().allSunk())
            {
                cout << "\n=== You win! ===\n";
                gameOver = true;
            }
            else if (res == ShotResult::Miss)
            {
                playerTurn = false; // miss → pass turn
            }
            // on Hit/Sunk you keep shooting
        }
        else
        {
            cout << "\nBot's turn...\n";

            // Ask bot AI for a move
            Position t = bot.getMove();

            int sunkIdx = -1;
            ShotResult res = human.getBoard().fireAt(t, sunkIdx);

            cout << "Bot fires at " << char('A' + t.col) << (t.row + 1) << ": ";
            if (res == ShotResult::Hit)
            {
                cout << "Hit!\n";
            }
            else if (res == ShotResult::Sunk)
            {
                cout << "Sunk one of your ships!\n";
                human.getBoard().revealPerimeterOf(sunkIdx);
            }
            else
            {
                cout << "Miss.\n";
            }

            // Let the bot learn
            bot.notifyShotResult(t, res, sunkIdx);

            cout << "\n(Your board)\n";
            human.getBoard().printForOwner();

            if (human.getBoard().allSunk())
            {
                cout << "\n=== Bot wins! ===\n";
                gameOver = true;
            }
            else if (res == ShotResult::Miss)
            {
                playerTurn = true; // miss → pass back
            }
            // on Hit/Sunk bot keeps shooting
        }
    }

    return 0;
}