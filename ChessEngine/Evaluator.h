#pragma once

#include "State.h"

class Evaluator {
    static int getPawnEquivalent(char piece);
public:
    static double getEvaluation(State& state);
};
