#pragma once

#include <memory>      
#include <functional>  
#include <cstddef>
#include <iterator>  

template<typename T>
class Sequence {
public:
    using SeqUPtr = std::unique_ptr<Sequence<T>>;

    virtual ~Sequence() = default;

    virtual std::size_t GetLength() const = 0;          
    virtual T Get(std::size_t idx) const = 0;           
    virtual T GetFirst() const = 0;                    
    virtual T GetLast() const = 0;                      
    virtual SeqUPtr GetSubsequence(std::size_t start, std::size_t end) const = 0;

    virtual SeqUPtr Clone() const = 0;                 
    virtual Sequence<T>* Instance() = 0;               

    virtual void Append(const T& v) = 0;             
    virtual void Prepend(const T& v) = 0;             
    virtual void InsertAt(const T& v, std::size_t idx) = 0;
    virtual Sequence<T>* Concat(Sequence<T>* other) = 0;

    virtual SeqUPtr Append(const T& v) const = 0;      
    virtual SeqUPtr Prepend(const T& v) const = 0;     
    virtual SeqUPtr InsertAt(const T& v, std::size_t idx) const = 0;
    virtual SeqUPtr Concat(const Sequence<T>* other) const = 0;

    virtual T& operator[](std::size_t idx) = 0;         
    virtual const T& operator[](std::size_t idx) const = 0;

    virtual bool TryGet(std::size_t idx, T& out) const = 0;    
    virtual bool TryGetFirst(T& out) const = 0;                
    virtual bool TryGetLast(T& out) const = 0;                 
    virtual bool TryFind(std::function<bool(const T&)> pred, T& out) const = 0;

    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const T*;
        using reference         = const T&;

        Iterator(const Sequence<T>* seq, std::size_t pos)
          : seq_(seq), pos_(pos) {}
        Iterator& operator++() { ++pos_; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++*this; return tmp; }
        bool operator==(const Iterator& o) const {
            return seq_ == o.seq_ && pos_ == o.pos_;
        }
        bool operator!=(const Iterator& o) const {
            return !(*this == o);
        }
        const T& operator*() const {
            return seq_->Get(pos_);
        }
    private:
        const Sequence<T>* seq_;
        std::size_t pos_;
    };

    Iterator begin() const { return Iterator(this, 0); }
    Iterator end()   const { return Iterator(this, GetLength()); }
};
