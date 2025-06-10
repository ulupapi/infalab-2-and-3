#pragma once

#include "ListSequence.hpp"

// Изменяемая последовательность на основе списка
template<typename T>
class MutableListSequence
  : public ListSequence<T, MutableListSequence<T>>
{
public:
    using ListSequence<T, MutableListSequence<T>>::ListSequence;
};
