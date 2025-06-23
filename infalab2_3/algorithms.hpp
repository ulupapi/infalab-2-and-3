#pragma once

#include "Sequence.hpp"
#include "MutableArraySequence.hpp"
#include <functional>
#include <memory>
#include <utility>
#include <algorithm>

template<typename T, typename U>
typename Sequence<U>::SeqUPtr Map(
    const Sequence<T>& src,
    std::function<U(const T&)> f)
{
    auto out = std::make_unique<MutableArraySequence<U>>();
    std::size_t n = src.GetLength();
    for (std::size_t i = 0; i < n; ++i)
        out->Append(f(src.Get(i)));
    return out;
}

template<typename T, typename U>
typename Sequence<U>::SeqUPtr FlatMap(
    const Sequence<T>& src,
    std::function<typename Sequence<U>::SeqUPtr(const T&)> f)
{
    auto out = std::make_unique<MutableArraySequence<U>>();
    std::size_t n = src.GetLength();
    for (std::size_t i = 0; i < n; ++i) {
        auto part = f(src.Get(i));
        std::size_t m = part->GetLength();
        for (std::size_t j = 0; j < m; ++j)
            out->Append(part->Get(j));
    }
    return out;
}

template<typename T>
typename Sequence<T>::SeqUPtr Where(
    const Sequence<T>& src,
    std::function<bool(const T&)> pred)
{
    auto out = std::make_unique<MutableArraySequence<T>>();
    std::size_t n = src.GetLength();
    for (std::size_t i = 0; i < n; ++i) {
        const T& v = src.Get(i);
        if (pred(v)) out->Append(v);
    }
    return out;
}

template<typename T, typename U>
U Reduce(
    const Sequence<T>& src,
    U init,
    std::function<U(const U&, const T&)> f)
{
    U acc = init;
    std::size_t n = src.GetLength();
    for (std::size_t i = 0; i < n; ++i)
        acc = f(acc, src.Get(i));
    return acc;
}

template<typename T>
T Find(
    const Sequence<T>& src,
    std::function<bool(const T&)> pred)
{
    std::size_t n = src.GetLength();
    for (std::size_t i = 0; i < n; ++i)
        if (pred(src.Get(i))) return src.Get(i);
    throw std::runtime_error("Find: no matching element");
}

template<typename T>
bool TryFind(
    const Sequence<T>& src,
    std::function<bool(const T&)> pred,
    T& out)
{
    std::size_t n = src.GetLength();
    for (std::size_t i = 0; i < n; ++i) {
        const T& v = src.Get(i);
        if (pred(v)) { out = v; return true; }
    }
    return false;
}

template<typename T>
bool ContainsSubsequence(
    const Sequence<T>& src,
    const Sequence<T>& pat)
{
    std::size_t n = src.GetLength(), m = pat.GetLength();
    if (m == 0) return true;
    for (std::size_t i = 0; i + m <= n; ++i) {
        bool ok = true;
        for (std::size_t j = 0; j < m; ++j) {
            if (src.Get(i + j) != pat.Get(j)) { ok = false; break; }
        }
        if (ok) return true;
    }
    return false;
}

template<typename A, typename B>
typename Sequence<std::pair<A,B>>::SeqUPtr Zip(
    const Sequence<A>& a,
    const Sequence<B>& b)
{
    auto out = std::make_unique<MutableArraySequence<std::pair<A,B>>>();
    std::size_t n = std::min(a.GetLength(), b.GetLength());
    for (std::size_t i = 0; i < n; ++i)
        out->Append({a.Get(i), b.Get(i)});
    return out;
}

template<typename A, typename B>
std::pair<
  typename Sequence<A>::SeqUPtr,
  typename Sequence<B>::SeqUPtr
> Unzip(const Sequence<std::pair<A,B>>& src)
{
    auto ua = std::make_unique<MutableArraySequence<A>>();
    auto ub = std::make_unique<MutableArraySequence<B>>();
    std::size_t n = src.GetLength();
    for (std::size_t i = 0; i < n; ++i) {
        const auto &p = src.Get(i);
        ua->Append(p.first);
        ub->Append(p.second);
    }
    return {std::move(ua), std::move(ub)};
}

template<typename T>
typename Sequence<Sequence<T>*>::SeqUPtr Split(
    const Sequence<T>& src,
    std::function<bool(const T&)> delim)
{
    using OutPtr = Sequence<T>*;
    auto out = std::make_unique<MutableArraySequence<OutPtr>>();
    auto cur = std::make_unique<MutableArraySequence<T>>();
    std::size_t n = src.GetLength();
    for (std::size_t i = 0; i < n; ++i) {
        const T& v = src.Get(i);
        if (delim(v)) {
            out->Append(cur->Clone().release());
            cur = std::make_unique<MutableArraySequence<T>>();
        } else {
            cur->Append(v);
        }
    }
    out->Append(cur->Clone().release());
    return out;
}

template<typename T>
typename Sequence<T>::SeqUPtr Slice(
    const Sequence<T>& src,
    int index,
    std::size_t cnt,
    const Sequence<T>* insert = nullptr)
{
    int n = static_cast<int>(src.GetLength());
    if (index < 0) index += n;
    if (index < 0 || index > n)
        throw std::out_of_range("Slice: bad index");
    auto out = std::make_unique<MutableArraySequence<T>>();
    for (int i = 0; i < index; ++i)
        out->Append(src.Get(i));
    if (insert) {
        std::size_t m = insert->GetLength();
        for (std::size_t j = 0; j < m; ++j)
            out->Append(insert->Get(j));
    }
    for (int i = index + static_cast<int>(cnt); i < n; ++i)
        out->Append(src.Get(i));
    return out;
}
