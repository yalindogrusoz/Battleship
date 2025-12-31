/*
Name: Yalin Dogrusoz
OS: MacOS
Proj: Battleship Final Project
File: Position.h
Purpose: Board position (row, col) on a 10x10 grid.
*/

#pragma once

struct Position
{
    int row;  // 0..9
    int col;  // 0..9
};

// simple helpers
// returns true if both Position objects' row AND column matcb
inline bool operator==(const Position& a, const Position& b)
{
    return a.row == b.row && a.col == b.col;
}

// the opposite of the inline func above just in case
inline bool operator!=(const Position& a, const Position& b)
{
    return !(a == b);
}