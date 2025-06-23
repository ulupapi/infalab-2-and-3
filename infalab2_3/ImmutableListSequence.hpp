#pragma once

#include "ListSequence.hpp"
#include <stdexcept>


template<typename T>
class ImmutableListSequence
  : public ListSequence<T, ImmutableListSequence<T>>
{
    using Base = ListSequence<T, ImmutableListSequence<T>>;

public:
    using Base::Base;            
    using Base::Append;          
    using Base::Prepend;       
    using Base::InsertAt;       
    using Base::Concat;        

    // Блокируем мутабельные методы 
    void Append(const T&) override            { throw std::logic_error("Immutable"); }
    void Prepend(const T&) override           { throw std::logic_error("Immutable"); }
    void InsertAt(const T&, std::size_t) override  { throw std::logic_error("Immutable"); }
    Sequence<T>* Concat(Sequence<T>*) override { throw std::logic_error("Immutable"); }
};
