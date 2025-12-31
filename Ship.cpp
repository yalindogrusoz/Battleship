/*
Name: Yalin Dogrusoz
OS: MacOS
Proj: Battleship Final Project
File: Ship.cpp
Purpose: Ship methods: placement, hits, sunk, perimeter.
*/

#include "Ship.h"
using namespace std;

namespace
{
    inline bool inBounds(const Position& p) // validatoin function
    {
        return p.row >= 0 && p.row < 10 && p.col >= 0 && p.col < 10;
    }

    inline bool containsPos(const vector<Position>& v, const Position& p) // checks if p in v
    {
        for (const auto& x : v)
        {
            if (x.row == p.row && x.col == p.col) return true;
        }
        return false;
    }
}

Ship::Ship(int size) 
    : m_size(size)
{
    if (m_size < 0) m_size = 0; // dont allow negative size
    m_cells.clear();
    m_hits.clear();
}

int Ship::size() const //getter for size
{
    return m_size;
}

void Ship::setCells(const vector<Position>& cells)
{
    m_cells = cells;
    m_size  = static_cast<int>(m_cells.size());
    m_hits.assign(m_size, false); // initializes m_hits with all false, as no cell is hit yet
}

bool Ship::occupies(const Position& p) // returns whether or not the ship has a square on that position
{
    return containsPos(m_cells, p);
}

bool Ship::hit(const Position& p) // updates m_hits if p is in m_cells (if p is a square of the ship) and returns true if it was
{
    for (size_t i = 0; i < m_cells.size(); ++i)
    {
        if (m_cells[i].row == p.row && m_cells[i].col == p.col)
        {
            m_hits[i] = true;
            return true;
        }
    }
    return false;
}

bool Ship::isSunk() const // returns true if and only if all entries are true
{
    if (m_hits.empty()) return false;
    for (bool h : m_hits)
    {
        if (!h) return false;
    }
    return true;
}

const vector<Position>& Ship::cells() const // getter for the cells
{
    return m_cells;
}

vector<Position> Ship::perimeter() const
{
    vector<Position> out; // builds a vector of Position objects of neighbours to a ship's cells. like if a 10x10 board has coords[r,c] for r and c elementof {0,1,...,9} then the ship [5,6],[5,7] a 2x1 ship would have the neighbours [4,5],[4,6],[4,7],[4,8],[5,5],[5,8],[6,5],[6,6],[6,7],[6,8]

    // add neighbors around every ship cell
    for (const auto& c : m_cells)
    {
        for (int dr = -1; dr <= 1; ++dr) // for every square to the left, middle and right of the cell
        {
            for (int dc = -1; dc <= 1; ++dc) // for every square up, middle and down of cell
            {
                Position n{ c.row + dr, c.col + dc }; // define a new position n
                if (!inBounds(n)) continue; // if n is out of bounds disregard it

                // skip actual ship cells; we want only the ring around it
                if (containsPos(m_cells, n)) continue;

                // if its not already in out, then add it
                if (!containsPos(out, n)) out.push_back(n);
            }
        }
    }
    return out;
}