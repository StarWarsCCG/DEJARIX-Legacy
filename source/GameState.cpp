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
    {
        auto instance = instances[i];
        _cardStateByInstanceId[instance.id] = instance.state;
        _deltas.push_back({stepIndex, instance});
    }

    _millisecondsByStep.push_back(elapsed);
}
