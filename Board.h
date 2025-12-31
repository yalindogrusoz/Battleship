/*
Name: Yalin Dogrusoz
OS: MacOS
Proj: Battleship Final Project
File: Board.h
Purpose: 10x10 board, fleet, placement, fire, display.
*/

#pragma once
#include <vector>
#include <string>
#include "Types.h"
#include "Position.h"
#include "Ship.h"
using namespace std;

class Board
{
public:
    Board();

    // placement
    bool placeShip(const Position& start, Direction dir, int size);
    bool randomizeFleet(const vector<int>& sizes);

    // gameplay
    ShotResult fireAt(const Position& p, int& sunkIndex); // sunkIndex = -1 if none
    bool allSunk() const;

    // reveal ring around a sunk ship as misses
    void revealPerimeterOf(int shipIndex);

    // printing
    void printForOwner() const;
    void printForOpponent() const;

    // query
    bool inBounds(const Position& p) const;

private:
    Cell m_grid[10][10];
    vector<Ship> m_ships;
    int m_remainingCells; // total intact ship cells

    // helpers
    bool fits(const Position& start, Direction dir, int size) const;
    bool areaClear(const Position& start, Direction dir, int size) const; // no overlap, no adjacency (incl diag)
    vector<Position> cellsFor(const Position& start, Direction dir, int size) const;
    bool isWater(const Position& p) const;
    void markMissIfWater(const Position& p);
    bool alreadyShot(const Position& p) const;
};