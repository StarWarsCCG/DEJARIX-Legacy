#include "GameState.hpp"

void GameState::apply(CardInstance instance)
{
    applyAll(&instance, 1);
}

void GameState::applyAll(const CardInstance* instances, int instanceCount)
{
    int elapsed = _stopwatch.elapsed();
    auto stepIndex = (int)_millisecondsByStep.size();

    for (int i = 0; i < instanceCount; ++i)
        _deltas.push_back({stepIndex, instances[i]});

    _millisecondsByStep.push_back(elapsed);
}
