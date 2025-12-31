/*
Name: Yalin Dogrusoz
OS: MacOS
Proj: Battleship Final Project
File: BotAI.cpp
Purpose: Implementation for a simple hunt/target bot.
*/

#include "BotAI.h"
#include <cstdlib>
#include <ctime>
using namespace std;

BotAI::BotAI()
{
    // seed once
    static bool seeded = false;
    if (!seeded)
    {
        srand(time(0));
        seeded = true;
    }

    // start with all board squares available (10x10)
    m_available.clear();
    for (int r = 0; r < 10; ++r)
    {
        for (int c = 0; c < 10; ++c)
        {
            m_available.push_back({ r, c });
        }
    }
    m_hits.clear();
    m_queue.clear();
}

Position BotAI::chooseMove()
{
    // Prefer queued targets (adjacent or line-extensions)
    while (!m_queue.empty())
    {
        Position p = m_queue.back();
        m_queue.pop_back();
        if (inAvailable(p))
        {
            removeFromAvailable(p);
            return p;
        }
        // if not available anymore, skip it and keep popping
    }

    // Otherwise, shoot a random available square
    if (m_available.empty())
    {
        return { 0, 0 }; // failsafe, shouldn't happen
    }
    int idx = rand() % m_available.size();
    Position p = m_available[idx];
    removeFromAvailable(p);
    return p;
}

void BotAI::updateAfterShot(const Position& p, ShotResult result, int /*sunkIndex*/)
{
    // nothing to do on miss
    if (result == ShotResult::Miss) return;

    if (result == ShotResult::Hit)
    {
        // remember this hit and queue neighbors
        m_hits.push_back(p);
        addAdjacents(p);

        // if we have two or more hits, try to infer orientation and extend in-line
        if (m_hits.size() >= 2)
        {
            focusLine();
        }
        return;
    }

    if (result == ShotResult::Sunk)
    {
        // ship is done → clear targeting info and start hunting again
        m_hits.clear();
        m_queue.clear();
        return;
    }
}

// helpers

bool BotAI::inBounds(const Position& p) const
{
    return p.row >= 0 && p.row < 10 && p.col >= 0 && p.col < 10;
}

bool BotAI::inAvailable(const Position& p) const
{
    for (const auto& a : m_available)
    {
        if (a.row == p.row && a.col == p.col) return true;
    }
    return false;
}

void BotAI::removeFromAvailable(const Position& p)
{
    for (auto it = m_available.begin(); it != m_available.end(); ++it)
    {
        if (it->row == p.row && it->col == p.col)
        {
            m_available.erase(it);
            return;
        }
    }
}

void BotAI::enqueueIfAvailable(const Position& p)
{
    if (!inBounds(p)) return;
    if (!inAvailable(p)) return;
    m_queue.push_back(p);
}

void BotAI::addAdjacents(const Position& p)
{
    // up, down, left, right — adds only if still unshot
    enqueueIfAvailable({ p.row - 1, p.col });
    enqueueIfAvailable({ p.row + 1, p.col });
    enqueueIfAvailable({ p.row, p.col - 1 });
    enqueueIfAvailable({ p.row, p.col + 1 });
}

void BotAI::focusLine()
{
    // If we have at least two hits, and they share row or col,
    // only extend along that line (don’t waste guesses sideways).
    const Position& a = m_hits[0];
    const Position& b = m_hits[1];

    // same row → horizontal line
    if (a.row == b.row)
    {
        int row = a.row;
        // find min/max col hit
        int minc = a.col, maxc = a.col;
        for (const auto& h : m_hits)
        {
            if (h.row == row)
            {
                if (h.col < minc) minc = h.col;
                if (h.col > maxc) maxc = h.col;
            }
        }
        // extend ends if possible
        enqueueIfAvailable({ row, minc - 1 });
        enqueueIfAvailable({ row, maxc + 1 });
        return;
    }

    // same col → vertical line
    if (a.col == b.col)
    {
        int col = a.col;
        int minr = a.row, maxr = a.row;
        for (const auto& h : m_hits)
        {
            if (h.col == col)
            {
                if (h.row < minr) minr = h.row;
                if (h.row > maxr) maxr = h.row;
            }
        }
        enqueueIfAvailable({ minr - 1, col });
        enqueueIfAvailable({ maxr + 1, col });
        return;
    }

    // If the first two hits aren't aligned (rare due to guessing order),
    // just keep adjacent behavior — no special line focusing needed here.
}