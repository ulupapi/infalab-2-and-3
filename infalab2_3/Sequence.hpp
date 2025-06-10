#pragma once

#include <memory>      // для std::unique_ptr
#include <functional>  // для std::function
#include <cstddef>     // для std::size_t
#include <iterator>    // для std::input_iterator_tag

// Абстрактный интерфейс «последовательность».
// Поддерживает как изменяемые (mutable), так и неизменяемые (immutable) операции.
template<typename T>
class Sequence {
public:
    using SeqUPtr = std::unique_ptr<Sequence<T>>;

    virtual ~Sequence() = default;

    // --- Основное API (чтение) ---
    virtual std::size_t GetLength() const = 0;          // длина последовательности
    virtual T Get(std::size_t idx) const = 0;           // получить элемент по индексу
    virtual T GetFirst() const = 0;                     // первый элемент
    virtual T GetLast() const = 0;                      // последний элемент
    virtual SeqUPtr GetSubsequence(std::size_t start, std::size_t end) const = 0;
    // получить подпоследовательность [start..end], включая оба конца

    virtual SeqUPtr Clone() const = 0;                  // глубокое клонирование
    virtual Sequence<T>* Instance() = 0;                // новый пустой экземпляр того же типа

    // --- Мутабельные операции (меняют текущий объект) ---
    virtual void Append(const T& v) = 0;                // добавить в конец
    virtual void Prepend(const T& v) = 0;               // добавить в начало
    virtual void InsertAt(const T& v, std::size_t idx) = 0;
    // вставить v на позицию idx
    virtual Sequence<T>* Concat(Sequence<T>* other) = 0;
    // добавить в конец все элементы other, вернуть this

    // --- Иммутабельные операции (возвращают новую копию) ---
    virtual SeqUPtr Append(const T& v) const = 0;       // возвращает копию + v в конце
    virtual SeqUPtr Prepend(const T& v) const = 0;      // возвращает копию + v в начале
    virtual SeqUPtr InsertAt(const T& v, std::size_t idx) const = 0;
    virtual SeqUPtr Concat(const Sequence<T>* other) const = 0;
    // возвращает копию + элементы other

    // --- Операторы доступа ---
    virtual T& operator[](std::size_t idx) = 0;         // по ссылке для изменения
    virtual const T& operator[](std::size_t idx) const = 0;
    // по ссылке для чтения

    // --- Try-версия методов (без выбросов исключений) ---
    virtual bool TryGet(std::size_t idx, T& out) const = 0;     // безопасно получить по индексу
    virtual bool TryGetFirst(T& out) const = 0;                 // безопасно первый
    virtual bool TryGetLast(T& out) const = 0;                  // безопасно последний
    virtual bool TryFind(std::function<bool(const T&)> pred, T& out) const = 0;
    // найти первый элемент, удовлетворяющий pred

    // --- Итераторы (для range-based for) ---
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
