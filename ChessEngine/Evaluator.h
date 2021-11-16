#pragma once

#include "State.h"

class Evaluator {
    static constexpr double MAXIMUM_EVALUATION = 103;
    static int getPawnEquivalent(char piece);
public:
    static double getEvaluation(State& state);
    static double getMaximumEvaluation();
};
