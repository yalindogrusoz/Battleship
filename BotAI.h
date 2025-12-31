/*
Name: Yalin Dogrusoz
OS: MacOS
Proj: Battleship Final Project
File: BotAI.h
Purpose: Bot move logic (random until a hit, then target around it).
*/

#pragma once
#include <vector>
#include "Position.h"
#include "Types.h"
using namespace std;

class BotAI
{
public:
    BotAI();

    // Pick a square to shoot next
    Position chooseMove();

    // Tell the bot what happened so it can adjust
    void updateAfterShot(const Position& p, ShotResult result, int sunkIndex);

private:
    vector<Position> m_available;   // squares not shot yet
    vector<Position> m_hits;        // current cluster of hits (same ship, not sunk yet)
    vector<Position> m_queue;       // next targets to try (adjacent / in-line)

    // helpers
    bool inBounds(const Position& p) const;
    bool inAvailable(const Position& p) const;
    void removeFromAvailable(const Position& p);
    void enqueueIfAvailable(const Position& p);
    void addAdjacents(const Position& p);
    void focusLine(); // if we have 2+ hits in a line, only extend that line
};