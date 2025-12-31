/*
Name: Yalin Dogrusoz
OS: MacOS
Proj: Battleship Final Project
File: Types.h
Purpose: Common enums used across the game.
*/

#pragma once


// THIS FILE DEFINES ALL ENUMS USED
enum class Direction
{
    Up,
    Down,
    Left,
    Right
};

enum class Cell
{
    Water,   // unknown or empty
    Ship,    // hidden ship cell (owner view)
    Hit,
    Miss
};

enum class Mode
{
    Classic, // {5,4,3,3,2}
    Russian  // {4,3,3,2,2,2,1,1,1,1}
};

enum class ShotResult
{
    Miss,
    Hit, // got a square (part of the ship) but not the whole ship yet
    Sunk // got the last square, sinking the whole ship.
};