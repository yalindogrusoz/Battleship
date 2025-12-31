/*
Name: Yalin Dogrusoz
OS: MacOS
Proj: Battleship Final Project
File: Ship.h
Purpose: A ship: size, occupied cells, hit tracking, sunk check, perimeter.
*/

#pragma once
#include <vector>
#include "Position.h"
using namespace std;

class Ship
{
public:
    Ship(int size = 0); 

    int size() const;
    void setCells(const vector<Position>& cells); // set where the ship lives

    bool occupies(Position& p);       // true if p is on this ship
    bool hit(Position& p);                  // mark a hit if p is on it
    bool isSunk() const;

    const vector<Position>& cells() const;        // getter for cells
    vector<Position> perimeter() const;           // all Position's in the perimeter

private:
    int m_size;
    vector<Position> m_cells;
    vector<bool>     m_hits; // parallel array to m_cells which holds the information pertaining to whether or not each corresponding cell has been "hit" or not yet
};