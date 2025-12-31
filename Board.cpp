/*
Name: Yalin Dogrusoz
OS: MacOS
Proj: Battleship Final Project
File: Board.cpp
Purpose: Board logic: placement, fire, adjacency rules, print.
*/

#include "Board.h"
#include <iostream>
#include <random>
#include <ctime>
#include <cstdlib>
using namespace std;
// ANSI color codes for background colors
#define RESET   "\033[0m"
#define BLUE    "\033[44m"  // Water
#define BROWN "\033[48;5;94m" // 256-color brown
#define WHITE   "\033[47m"  // Miss
#define RED     "\033[41m"  // Hit

Board::Board()
    : m_remainingCells(0)
{
    for (int r = 0; r < 10; ++r)
    {
        for (int c = 0; c < 10; ++c)
        {
            m_grid[r][c] = Cell::Water; // initialize all to water
        }
    }
}

bool Board::inBounds(const Position& p) const // returns whether or not a given Position object p is in bounds or not
{
    return p.row >= 0 && p.row < 10 && p.col >= 0 && p.col < 10;
}

vector<Position> Board::cellsFor(const Position& start, Direction dir, int size) const // example: (position p obejct with row 5 col 5, Direction::Up, 3)
{
    vector<Position> cells;
    cells.reserve(size);
    for (int i = 0; i < size; ++i)
    {
        Position p = start;
        if (dir == Direction::Up)    p.row -= i;
        if (dir == Direction::Down)  p.row += i;
        if (dir == Direction::Left)  p.col -= i;
        if (dir == Direction::Right) p.col += i;
        cells.push_back(p);
    }
    return cells;
}

bool Board::fits(const Position& start, Direction dir, int size) const // returns whether or not all cells in a given start point direction and size are in bounds or not
{
    auto cells = cellsFor(start, dir, size);
    for (const auto& p : cells)
    {
        if (!inBounds(p)) return false;
    }
    return true;
}

bool Board::isWater(const Position& p) const // returns whether or not a given p is water
{
    return m_grid[p.row][p.col] == Cell::Water;
}

bool Board::areaClear(const Position& start, Direction dir, int size) const // returns whether or not a boat is "placable" in a given startpoint direction and size. it is not placable if a neighboring square has a ship, a ship that was hit, or if all squares of the boat aren't in bounds.
{
    auto cells = cellsFor(start, dir, size);

    // check overlap and adjacency around each candidate cell
    for (const auto& c : cells)
    {
        if (!inBounds(c)) return false;

        // 8-neighborhood + self
        for (int dr = -1; dr <= 1; ++dr) 
        {
            for (int dc = -1; dc <= 1; ++dc)
            {
                Position n{ c.row + dr, c.col + dc };
                if (!inBounds(n)) continue;

                // any existing ship cell or hit on ship means not clear
                if (m_grid[n.row][n.col] == Cell::Ship || m_grid[n.row][n.col] == Cell::Hit) // a boat aren't meant to be adjacent to each other, there must at least be one square. therefore we check to reject if any neighbours are ships or sunk squares.
                    return false;
            }
        }
    }
    return true;
}

bool Board::placeShip(const Position& start, Direction dir, int size) // does all checks calling helper functions and if valid, changes the position objects in grid
{
    if (size <= 0) return false;
    if (!fits(start, dir, size)) return false;
    if (!areaClear(start, dir, size)) return false;

    auto cells = cellsFor(start, dir, size);
    Ship s(size);
    s.setCells(cells);
    m_ships.push_back(s);

    for (const auto& p : cells)
    {
        m_grid[p.row][p.col] = Cell::Ship;
    }
    m_remainingCells += size;
    return true;
}

bool Board::randomizeFleet(const vector<int>& sizes)
{
    srand((unsigned)time(NULL));

    for (size_t s = 0; s < sizes.size(); ++s)
    {
        int len = sizes[s];
        bool placed = false;
        int attempts = 0;

        // 1) pick random square until empty
        // 2) collect all directions that fit
        // 3) if none, pick another square; if some, choose one randomly and place
        while (!placed && attempts < 5000) // it is combinatorially impossible for boats NOT to fit in any placement, hence this is valid
        {
            ++attempts;

            int r = rand() % 10;
            int c = rand() % 10;
            if (m_grid[r][c] != Cell::Water) continue;

            Position start;
            start.row = r;
            start.col = c;

            Direction options[4];
            int count = 0;

            if (fits(start, Direction::Up,    len) && areaClear(start, Direction::Up,    len)) options[count++] = Direction::Up;
            if (fits(start, Direction::Down,  len) && areaClear(start, Direction::Down,  len)) options[count++] = Direction::Down;
            if (fits(start, Direction::Left,  len) && areaClear(start, Direction::Left,  len)) options[count++] = Direction::Left;
            if (fits(start, Direction::Right, len) && areaClear(start, Direction::Right, len)) options[count++] = Direction::Right;

            if (count == 0) continue; // no direction works from this square

            int pickIndex = rand() % count;
            Direction pick = options[pickIndex];

            if (placeShip(start, pick, len)) placed = true;
        }

        if (!placed) return false; // failed to place this ship
    }

    return true;
}


bool Board::alreadyShot(const Position& p) const
{
    return m_grid[p.row][p.col] == Cell::Hit || m_grid[p.row][p.col] == Cell::Miss;
}

ShotResult Board::fireAt(const Position& p, int& sunkIndex)
{
    sunkIndex = -1;
    if (!inBounds(p)) return ShotResult::Miss; // out of bounds -> treat as miss
    if (alreadyShot(p)) return ShotResult::Miss; // ignore repeats

    if (m_grid[p.row][p.col] == Cell::Ship)
    {
        m_grid[p.row][p.col] = Cell::Hit;
        --m_remainingCells;

        // find which ship was hit and update it
        for (size_t i = 0; i < m_ships.size(); ++i)
        {
            if (m_ships[i].hit(p))
            {
                if (m_ships[i].isSunk())
                {
                    sunkIndex = static_cast<int>(i);
                    return ShotResult::Sunk;
                }
                return ShotResult::Hit;
            }
        }
        // should not reach here
        return ShotResult::Hit;
    }
    else
    {
        m_grid[p.row][p.col] = Cell::Miss;
        return ShotResult::Miss;
    }
}

void Board::markMissIfWater(const Position& p)
{
    if (!inBounds(p)) return;
    if (m_grid[p.row][p.col] == Cell::Water) m_grid[p.row][p.col] = Cell::Miss;
}

void Board::revealPerimeterOf(int shipIndex) // when a ship is hit we reveal the perimeter by changing all perimetering squares to miss
{
    if (shipIndex < 0 || shipIndex >= static_cast<int>(m_ships.size())) return; // exit if shipIndex is somehow negative
    vector<Position> ring = m_ships[shipIndex].perimeter();
    for (const auto& p : ring)
    {
        markMissIfWater(p);
    }
}

bool Board::allSunk() const
{
    return m_remainingCells == 0;
}

void Board::printForOwner() const
{
    cout << "   A B C D E F G H I J\n";
    for (int r = 0; r < 10; ++r)
    {
        cout << (r + 1 < 10 ? " " : "") << r + 1 << " ";
        for (int c = 0; c < 10; ++c)
        {
            if (m_grid[r][c] == Cell::Water)      cout << BLUE  << "  " << RESET;
            else if (m_grid[r][c] == Cell::Ship)  cout << BROWN << "  " << RESET;
            else if (m_grid[r][c] == Cell::Miss)  cout << WHITE << "  " << RESET;
            else if (m_grid[r][c] == Cell::Hit)   cout << RED   << "  " << RESET;
        }
        cout << "\n";
    }
}

void Board::printForOpponent() const
{
    cout << "   A B C D E F G H I J\n";
    for (int r = 0; r < 10; ++r)
    {
        cout << (r + 1 < 10 ? " " : "") << r + 1 << " ";
        for (int c = 0; c < 10; ++c)
        {
            if (m_grid[r][c] == Cell::Hit)        cout << RED   << "  " << RESET;
            else if (m_grid[r][c] == Cell::Miss)  cout << WHITE << "  " << RESET;
            else                                  cout << BLUE  << "  " << RESET; // hide ships as water
        }
        cout << "\n";
    }
}