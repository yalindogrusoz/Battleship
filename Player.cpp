/*
Name: Yalin Dogrusoz
OS: MacOS
Proj: Battleship Final Project
File: Player.cpp
Purpose: Player methods for setup, moves, and tracking.
*/

#include "Player.h"
#include <iostream>
using namespace std;

Player::Player(const string& name, Type type)
    : m_name(name), m_type(type)
{
}

void Player::placeFleet(const vector<int>& sizes, bool manualPlacement)
{
    // If this player is a bot OR we don't want manual placement,
    // just randomly place all ships and skip the rest.
    if (m_type == Type::Bot || !manualPlacement)
    {
        m_board.randomizeFleet(sizes);
    }
    else
    {
        // Go through each ship size in the list
        for (int len : sizes)
        {
            bool placed = false;

            // Keep looping until this ship is placed successfully
            while (!placed)
            {
                // Show the board so the player can see what they already placed
                m_board.printForOwner();

                // Tell the player which ship they are placing
                cout << m_name << ", place your ship of length " << len << ".\n";

                // Ask where the ship should start
                cout << "Enter start position (e.g., A5): ";
                string posInput;
                cin >> posInput;

                // If they typed something too short, skip and try again
                if (posInput.size() < 2) continue;

                // Convert the input to a Position (row/col on board)
                Position start;
                start.col = toupper(posInput[0]) - 'A'; // 'A' becomes 0, 'B' becomes 1, etc.
                start.row = stoi(posInput.substr(1)) - 1; // number part becomes row (0-based)

                // Ask which way the ship should face
                cout << "Direction (u/d/l/r): ";
                char dirChar;
                cin >> dirChar;

                // find which Direction enum matches the letter
                Direction dir;
                if (dirChar == 'u') dir = Direction::Up;
                else if (dirChar == 'd') dir = Direction::Down;
                else if (dirChar == 'l') dir = Direction::Left;
                else if (dirChar == 'r') dir = Direction::Right;
                else continue; // invalid input → try again

                // Try to put the ship there
                placed = m_board.placeShip(start, dir, len);

                // If it didn’t work, let the player know
                if (!placed)
                {
                    cout << "Invalid placement. Try again.\n";
                }
            }
        }
    }
}

//Getter methods below
Board& Player::getBoard()
{
    return m_board;
}

const Board& Player::getBoard() const
{
    return m_board;
}

Position Player::getMove()
{
    if (m_type == Type::Bot)
    {
        return m_ai.chooseMove();
    }
    else
    {
        cout << m_name << ", enter target (e.g., B7): ";
        string posInput;
        cin >> posInput;
        Position target;
        target.col = toupper(posInput[0]) - 'A'; // convert to uppercase, then get the number like 'B' - 'A' would be 1 which is the column index from 0 to 9
        target.row = stoi(posInput.substr(1)) - 1; // convert the row number 1-10 to an index 0-9 and uses stoi to turn string into int
        return target;
    }
}

// if bot, call updateAfterShot() for smart bot. implemented here for abstraction and encapsulation of the botAI class
void Player::notifyShotResult(const Position& p, ShotResult result, int sunkIndex)
{
    if (m_type == Type::Bot)
    {
        m_ai.updateAfterShot(p, result, sunkIndex);
    }
}

string Player::getName() const
{
    return m_name;
}

bool Player::isBot() const
{
    return m_type == Type::Bot;
}