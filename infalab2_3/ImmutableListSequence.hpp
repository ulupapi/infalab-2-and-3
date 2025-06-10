#pragma once

#include "ListSequence.hpp"
#include <stdexcept>

// Неизменяемая последовательность на основе списка
template<typename T>
class ImmutableListSequence
  : public ListSequence<T, ImmutableListSequence<T>>
{
    using Base = ListSequence<T, ImmutableListSequence<T>>;

public:
    using Base::Base;            // наследуем конструкторы
    using Base::Append;          // раскрываем const-версию Append
    using Base::Prepend;         // раскрываем const-версию Prepend
    using Base::InsertAt;        // раскрываем const-версию InsertAt
    using Base::Concat;          // раскрываем const-версию Concat

    // Блокируем мутабельные методы (non-const)
    void Append(const T&) override            { throw std::logic_error("Immutable"); }
    void Prepend(const T&) override           { throw std::logic_error("Immutable"); }
    void InsertAt(const T&, std::size_t) override  { throw std::logic_error("Immutable"); }
    Sequence<T>* Concat(Sequence<T>*) override { throw std::logic_error("Immutable"); }
};
