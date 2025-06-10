#pragma once

#include "Sequence.hpp"
#include "LinkedList.hpp"
#include <stdexcept>
#include <functional>

template<typename T, typename Derived>
class ListSequence : public Sequence<T> {
protected:
    LinkedList<T> data_;  // внутренний односвязный список

    using SeqUPtr = typename Sequence<T>::SeqUPtr;

    template<typename M, typename... Args>
    SeqUPtr cloneInvoke(M method, Args&&... args) const {
        auto cp = Clone();
        (static_cast<Derived*>(cp.get())->*method)(std::forward<Args>(args)...);
        return cp;
    }

public:
    using SeqUPtrPublic = SeqUPtr;

    ListSequence() = default;
    ListSequence(const T* p, std::size_t n) : data_(p, n) {}

    std::size_t GetLength() const override {
        return data_.GetLength();
    }
    T Get(std::size_t i) const override {
        return data_.Get(i);
    }
    T GetFirst() const override {
        return data_.GetFirst();
    }
    T GetLast() const override {
        return data_.GetLast();
    }

    SeqUPtr GetSubsequence(std::size_t l, std::size_t r) const override {
        if (l > r || r >= data_.GetLength())
            throw std::out_of_range("ListSequence::GetSubsequence: bad range");

        auto out = std::make_unique<Derived>();
        auto cur = data_.getHead();
        for (std::size_t idx = 0; idx <= r; ++idx) {
            if (idx >= l) {
                out->Append(cur->val);
            }
            cur = cur->next;
        }
        return out;
    }

    SeqUPtr Clone() const override {
        return std::make_unique<Derived>(static_cast<const Derived&>(*this));
    }
    Sequence<T>* Instance() override {
        return new Derived();
    }

    // -------------------
    // Mutable API (non-const)
    // -------------------
    void Append(const T& v) override {
        data_.Append(v);
    }
    void Prepend(const T& v) override {
        data_.Prepend(v);
    }
    void InsertAt(const T& v, std::size_t idx) override {
        data_.InsertAt(v, idx);
    }
    Sequence<T>* Concat(Sequence<T>* other) override {
        for (std::size_t i = 0; i < other->GetLength(); ++i) {
            data_.Append(other->Get(i));
        }
        return this;
    }

    // -------------------
    // Immutable API (non-const‐указатели, однозначно указываем на “не‐const” методы)
    // -------------------
    SeqUPtr Append(const T& v) const override {
        return cloneInvoke(static_cast<void (Derived::*)(const T&)>(&Derived::Append), v);
    }
    SeqUPtr Prepend(const T& v) const override {
        return cloneInvoke(static_cast<void (Derived::*)(const T&)>(&Derived::Prepend), v);
    }
    SeqUPtr InsertAt(const T& v, std::size_t idx) const override {
        return cloneInvoke(static_cast<void (Derived::*)(const T&, std::size_t)>(&Derived::InsertAt), v, idx);
    }
    SeqUPtr Concat(const Sequence<T>* other) const override {
        auto cp = Clone();
        for (std::size_t i = 0; i < other->GetLength(); ++i) {
            static_cast<Derived*>(cp.get())->Append(other->Get(i));
        }
        return cp;
    }

    T& operator[](std::size_t i) override {
        return data_[i];
    }
    const T& operator[](std::size_t i) const override {
        return data_.Get(i);
    }

    bool TryGet(std::size_t i, T& out) const override {
        if (i < data_.GetLength()) {
            out = data_.Get(i);
            return true;
        }
        return false;
    }
    bool TryGetFirst(T& out) const override {
        return TryGet(0, out);
    }
    bool TryGetLast(T& out) const override {
        std::size_t n = data_.GetLength();
        if (n == 0) return false;
        out = data_.Get(n - 1);
        return true;
    }
    bool TryFind(std::function<bool(const T&)> pred, T& out) const override {
        auto cur = data_.getHead();
        while (cur) {
            if (pred(cur->val)) {
                out = cur->val;
                return true;
            }
            cur = cur->next;
        }
        return false;
    }
};
