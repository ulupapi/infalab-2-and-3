#pragma once

#include "ArraySequence.hpp"
#include <stdexcept>

// Неизменяемая версия ArraySequence: все mutable-методы заблокированы
template<typename T>
class ImmutableArraySequence
  : public ArraySequence<T, ImmutableArraySequence<T>>
{
    using Base = ArraySequence<T, ImmutableArraySequence<T>>;

public:
    using SeqUPtr = typename Sequence<T>::SeqUPtr;
    using Base::Base;           // наследуем конструкторы
    using Base::Append;         // раскрываем const-версию Append из базового класса
    using Base::Prepend;        // раскрываем const-версию Prepend
    using Base::InsertAt;       // раскрываем const-версию InsertAt
    using Base::Concat;         // раскрываем const-версию Concat

    // --- Блокируем мутабельные методы (non-const) ---
    void Append(const T&) override            { throw std::logic_error("Immutable"); }
    void Prepend(const T&) override           { throw std::logic_error("Immutable"); }
    void InsertAt(const T&, std::size_t) override  { throw std::logic_error("Immutable"); }
    Sequence<T>* Concat(Sequence<T>*) override{ throw std::logic_error("Immutable"); }
};
