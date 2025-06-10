#pragma once

#include <memory>
#include <stdexcept>
#include <functional>
#include "Sequence.hpp"
#include "MutableListSequence.hpp"

// Очередь на основе Sequence<T>. По умолчанию внутри стоит MutableListSequence<T>.
template<typename T>
class QueueSequence : public Sequence<T> {
private:
    std::unique_ptr<Sequence<T>> seq_;

    // Псевдоним для «умного указателя» на Sequence<T>
    using SeqUPtr = typename Sequence<T>::SeqUPtr;

    // Вспомогательный шаблонный метод для immutable-версий (клонировать + вызвать мутирующий метод)
    template<typename M, typename... Args>
    SeqUPtr cloneInvoke(M method, Args&&... args) const {
        auto cp = Clone();
        (static_cast<QueueSequence<T>*>(cp.get())->*method)(std::forward<Args>(args)...);
        return cp;
    }

public:
    QueueSequence()
      : seq_(std::make_unique<MutableListSequence<T>>()) {}

    // --- Core read API ---
    std::size_t GetLength() const override {
        return seq_->GetLength();
    }
    T Get(std::size_t i) const override {
        return seq_->Get(i);
    }
    T GetFirst() const override {
        return seq_->GetFirst();
    }
    T GetLast() const override {
        return seq_->GetLast();
    }

    // --- Подпоследовательность: создаём новую очередь с элементами [l..r] ---
    SeqUPtr GetSubsequence(std::size_t l, std::size_t r) const override {
        auto sub = seq_->GetSubsequence(l, r);
        auto out = std::make_unique<QueueSequence<T>>();
        for (std::size_t i = 0; i < sub->GetLength(); ++i)
            out->Enqueue(sub->Get(i));
        return out;
    }

    // --- Clone / Instance ---
    SeqUPtr Clone() const override {
        auto cp = std::make_unique<QueueSequence<T>>();
        cp->seq_ = seq_->Clone();
        return cp;
    }
    Sequence<T>* Instance() override {
        return new QueueSequence<T>();
    }

    // --- Mutable API ---
    void Append(const T& v) override {
        Enqueue(v);
    }
    void Prepend(const T& v) override {
        seq_->Prepend(v);
    }
    void InsertAt(const T& v, std::size_t idx) override {
        seq_->InsertAt(v, idx);
    }
    Sequence<T>* Concat(Sequence<T>* other) override {
        for (std::size_t i = 0; i < other->GetLength(); ++i)
            Enqueue(other->Get(i));
        return this;
    }

    // --- Immutable API ---
    std::unique_ptr<Sequence<T>> Append(const T& v) const override {
        // Enqueue не перегружен, ambiguities нет
        return cloneInvoke(&QueueSequence::Enqueue, v);
    }
    std::unique_ptr<Sequence<T>> Prepend(const T& v) const override {
        auto cp = Clone();
        static_cast<QueueSequence<T>*>(cp.get())->seq_->Prepend(v);
        return cp;
    }
    std::unique_ptr<Sequence<T>> InsertAt(const T& v, std::size_t idx) const override {
        // Явно указываем non-const InsertAt
        return cloneInvoke(
            static_cast<void (QueueSequence::*)(const T&, std::size_t)>(&QueueSequence::InsertAt),
            v, idx
        );
    }
    std::unique_ptr<Sequence<T>> Concat(const Sequence<T>* other) const override {
        auto cp = Clone();
        for (std::size_t i = 0; i < other->GetLength(); ++i)
            static_cast<QueueSequence<T>*>(cp.get())->Enqueue(other->Get(i));
        return cp;
    }

    // --- Операции очереди ---
    void Enqueue(const T& v) {
        seq_->Append(v);
    }
    T Dequeue() {
        auto len = seq_->GetLength();
        if (len == 0)
            throw std::out_of_range("QueueSequence::Dequeue: пустая очередь");
        T front = seq_->GetFirst();
        if (len == 1) {
            seq_.reset(Instance());
        } else {
            seq_ = seq_->GetSubsequence(1, len - 1);
        }
        return front;
    }
    T Peek() const {
        if (seq_->GetLength() == 0)
            throw std::out_of_range("QueueSequence::Peek: пустая очередь");
        return seq_->GetFirst();
    }

    // --- Операторы доступа ---
    T& operator[](std::size_t) override {
        throw std::logic_error("operator[] не поддерживается в QueueSequence");
    }
    const T& operator[](std::size_t i) const override {
        return seq_->Get(i);
    }

    // --- Try-семантика ---
    bool TryGet(std::size_t i, T& out) const override {
        return seq_->TryGet(i, out);
    }
    bool TryGetFirst(T& out) const override {
        return seq_->TryGetFirst(out);
    }
    bool TryGetLast(T& out) const override {
        return seq_->TryGetLast(out);
    }
    bool TryFind(std::function<bool(const T&)> pred, T& out) const override {
        return seq_->TryFind(pred, out);
    }
};

// --- PartitionQueue: разбивает очередь на две по предикату ---
template<typename T>
std::pair<typename Sequence<T>::SeqUPtr,
          typename Sequence<T>::SeqUPtr>
PartitionQueue(const QueueSequence<T>& q, std::function<bool(const T&)> pred)
{
    auto yes = std::make_unique<QueueSequence<T>>();
    auto no  = std::make_unique<QueueSequence<T>>();
    for (std::size_t i = 0; i < q.GetLength(); ++i) {
        T v = q.Get(i);
        if (pred(v)) yes->Enqueue(v);
        else          no->Enqueue(v);
    }
    return { std::move(yes), std::move(no) };
}
