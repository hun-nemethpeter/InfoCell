#pragma once

#include "cells/Cells.h"
#include "util/Logging.h"

namespace synth {

class Solver
{
public:
    Solver(Logger& logger, const cells::Sensor& input, const cells::Sensor& output);

    void solve();

    Logger& logger;
    const cells::Sensor& m_input;
    const cells::Sensor& m_output;
};

/*

A program célja, hogy két pixelmátrixból megtalálja a transzformációs algoritmust.
A feladatok során mindig található egy transzformációs algoritmus, ami egyik mátrixból előállítja a másikat.

Első lépés, hogy a pixel mátrixban azonosítsuk az azonos színű alakzatokat. Az alakzat akár 1 pixeles is lehet.
Az egyik kitüntetett alakzat a "háttér" alakzat, ez jellemzően fekete. Sok példában, de nem mindben az alakzatok egy háttér előtt vannak.

Egy 2D alakzatot lehet forgatni, kicsinyíteni, nagyítani, mozgatni, átszínezni valamilyen szabály szerint.

Pixels => Shapes

*/

} // namespace synth