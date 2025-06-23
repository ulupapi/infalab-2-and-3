#pragma once

#include "Sequence.hpp"
#include "DynamicArray.hpp"
#include <stdexcept>
#include <utility>
#include <functional>

template<typename T, typename Derived>
class ArraySequence : public Sequence<T> {
protected:
    DynamicArray<T> data_;  

    template<typename M, typename... Args>
    std::unique_ptr<Sequence<T>> cloneInvoke(M method, Args&&... args) const {
        auto cp = Clone();
        (static_cast<Derived*>(cp.get())->*method)(std::forward<Args>(args)...);
        return cp;
    }

public:
    // --- Конструкторы ---
    ArraySequence() = default;
    ArraySequence(const T* p, std::size_t n)
      : data_(p, n) {}

    // --- Методы чтения ---
    std::size_t GetLength() const override {
        return data_.GetSize();
    }
    T Get(std::size_t i) const override {
        return data_.Get(i);
    }
    T GetFirst() const override {
        return data_.Get(0);
    }
    T GetLast() const override {
        return data_.Get(data_.GetSize() - 1);
    }

    // --- Подпоследовательность [l..r] ---
    std::unique_ptr<Sequence<T>> GetSubsequence(std::size_t l, std::size_t r) const override {
        if (l > r || r >= GetLength())
            throw std::out_of_range("ArraySequence::GetSubsequence: bad range");
        auto out = std::make_unique<Derived>();
        for (std::size_t i = l; i <= r; ++i) {
            out->Append(data_[i]);
        }
        return out;
    }

    // --- Клонирование ---
    std::unique_ptr<Sequence<T>> Clone() const override {
        return std::make_unique<Derived>(static_cast<const Derived&>(*this));
    }
    Sequence<T>* Instance() override {
        return new Derived();
    }

    // --- Мутабельные методы (по умолчанию запрещены, переопределяются в потомках) ---
    void Append(const T&) override {
        throw std::logic_error("ArraySequence: Append unavailable");
    }
    void Prepend(const T&) override {
        throw std::logic_error("ArraySequence: Prepend unavailable");
    }
    void InsertAt(const T&, std::size_t) override {
        throw std::logic_error("ArraySequence: InsertAt unavailable");
    }
    Sequence<T>* Concat(Sequence<T>*) override {
        throw std::logic_error("ArraySequence: Concat unavailable");
    }

    // --- Immutable API (через cloneInvoke) ---
    std::unique_ptr<Sequence<T>> Append(const T& v) const override {
        return cloneInvoke(static_cast<void (Derived::*)(const T&)>(&Derived::Append), v);
    }
    std::unique_ptr<Sequence<T>> Prepend(const T& v) const override {
        return cloneInvoke(static_cast<void (Derived::*)(const T&)>(&Derived::Prepend), v);
    }
    std::unique_ptr<Sequence<T>> InsertAt(const T& v, std::size_t idx) const override {
        return cloneInvoke(static_cast<void (Derived::*)(const T&, std::size_t)>(&Derived::InsertAt), v, idx);
    }
    std::unique_ptr<Sequence<T>> Concat(const Sequence<T>* other) const override {
        auto cp = Clone();
        for (std::size_t i = 0; i < other->GetLength(); ++i) {
            static_cast<Derived*>(cp.get())->Append(other->Get(i));
        }
        return cp;
    }

    // --- Операторы доступа ---
    T& operator[](std::size_t i) override {
        return data_[i];
    }
    const T& operator[](std::size_t i) const override {
        return data_[i];
    }

    // --- Try-версии ---
    bool TryGet(std::size_t i, T& out) const override {
        if (i < GetLength()) {
            out = data_[i];
            return true;
        }
        return false;
    }
    bool TryGetFirst(T& out) const override {
        return TryGet(0, out);
    }
    bool TryGetLast(T& out) const override {
        std::size_t n = GetLength();
        if (n == 0) return false;
        return TryGet(n - 1, out);
    }
    bool TryFind(std::function<bool(const T&)> pred, T& out) const override {
        for (std::size_t i = 0; i < GetLength(); ++i) {
            if (pred(data_[i])) {
                out = data_[i];
                return true;
            }
        }
        return false;
    }
};
