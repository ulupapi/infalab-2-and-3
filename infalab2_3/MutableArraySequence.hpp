#pragma once

#include "ArraySequence.hpp"
#include "DynamicArray.hpp"
#include <stdexcept>

template<typename T>
class MutableArraySequence
  : public ArraySequence<T, MutableArraySequence<T>>
{
    using Base = ArraySequence<T, MutableArraySequence<T>>;
public:
    using Base::Base;  

    void Append(const T& v) override {
        auto n = this->GetLength();
        this->data_.Resize(n + 1);
        this->data_.Set(n, v);
    }

    void Prepend(const T& v) override {
        auto n = this->GetLength();
        DynamicArray<T> tmp(n + 1);
        tmp.Set(0, v);
        for (std::size_t i = 0; i < n; ++i)
            tmp.Set(i + 1, this->data_.Get(i));
        this->data_ = std::move(tmp);
    }

    void InsertAt(const T& v, std::size_t idx) override {
        auto n = this->GetLength();
        if (idx > n) throw std::out_of_range("MutableArraySequence::InsertAt: bad idx");
        DynamicArray<T> tmp(n + 1);
        for (std::size_t i = 0; i < idx; ++i)
            tmp.Set(i, this->data_.Get(i));
        tmp.Set(idx, v);
        for (std::size_t i = idx; i < n; ++i)
            tmp.Set(i + 1, this->data_.Get(i));
        this->data_ = std::move(tmp);
    }

    Sequence<T>* Concat(Sequence<T>* other) override {
        for (std::size_t i = 0; i < other->GetLength(); ++i)
            Append(other->Get(i));
        return this;
    }
};
