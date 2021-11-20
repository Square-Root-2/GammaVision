#pragma once

#include "State.h"

class Evaluator {
    static constexpr double MAXIMUM_EVALUATION = 103;
public:
    static double getEvaluation(State& state);
    static double getMaximumEvaluation();
    static int getPawnEquivalent(char piece);
};
