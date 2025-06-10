#pragma once

#include <cstddef>     // для std::size_t
#include <stdexcept>   // для std::out_of_range
#include <initializer_list>
#include <utility>     // для std::move

// Шаблон динамического массива (аналог std::vector)
template<typename T>
class DynamicArray {
private:
    T* data_;               // указатель на выделенную память
    std::size_t size_;      // текущее число элементов

public:
    // --- Конструкторы и деструктор ---
    DynamicArray() : data_(nullptr), size_(0) {}

    // Конструктор из сырого массива
    DynamicArray(const T* items, std::size_t count)
      : data_(new T[count]), size_(count)
    {
        for (std::size_t i = 0; i < size_; ++i)
            data_[i] = items[i];
    }

    // Конструктор из initializer_list, например: {1,2,3}
    DynamicArray(std::initializer_list<T> init)
      : data_(new T[init.size()]), size_(init.size())
    {
        std::size_t i = 0;
        for (const T& v : init)
            data_[i++] = v;
    }

    // Конструктор с заданным размером (все элементы default-инициализируются)
    explicit DynamicArray(std::size_t size)
      : data_(new T[size]), size_(size)
    {
        for (std::size_t i = 0; i < size_; ++i)
            data_[i] = T();
    }

    // Конструктор копирования
    DynamicArray(const DynamicArray& other)
      : data_(new T[other.size_]), size_(other.size_)
    {
        for (std::size_t i = 0; i < size_; ++i)
            data_[i] = other.data_[i];
    }

    // Конструктор перемещения
    DynamicArray(DynamicArray&& o) noexcept
      : data_(o.data_), size_(o.size_)
    {
        o.data_ = nullptr;
        o.size_ = 0;
    }

    ~DynamicArray() {
        delete[] data_;
    }

    // --- Операторы присваивания ---
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] data_;
            size_ = other.size_;
            data_ = new T[size_];
            for (std::size_t i = 0; i < size_; ++i)
                data_[i] = other.data_[i];
        }
        return *this;
    }

    DynamicArray& operator=(DynamicArray&& o) noexcept {
        if (this != &o) {
            delete[] data_;
            data_ = o.data_;
            size_ = o.size_;
            o.data_ = nullptr;
            o.size_ = 0;
        }
        return *this;
    }

    // --- Доступ к элементам ---
    T Get(std::size_t idx) const {
        if (idx >= size_) throw std::out_of_range("DynamicArray::Get: bad index");
        return data_[idx];
    }

    void Set(std::size_t idx, const T& value) {
        if (idx >= size_) throw std::out_of_range("DynamicArray::Set: bad index");
        data_[idx] = value;
    }
    void Set(std::size_t idx, T&& value) {
        if (idx >= size_) throw std::out_of_range("DynamicArray::Set: bad index");
        data_[idx] = std::move(value);
    }

    // --- Размер массива ---
    std::size_t GetSize() const {
        return size_;
    }

    void Resize(std::size_t newSize) {
        T* newData = new T[newSize];
        std::size_t n = (newSize < size_ ? newSize : size_);
        for (std::size_t i = 0; i < n; ++i)
            newData[i] = std::move(data_[i]);
        for (std::size_t i = n; i < newSize; ++i)
            newData[i] = T();
        delete[] data_;
        data_ = newData;
        size_ = newSize;
    }

    // --- Операторы индексации ---
    T& operator[](std::size_t idx) {
        if (idx >= size_) throw std::out_of_range("DynamicArray::operator[]: bad index");
        return data_[idx];
    }
    const T& operator[](std::size_t idx) const {
        if (idx >= size_) throw std::out_of_range("DynamicArray::operator[] const: bad index");
        return data_[idx];
    }
};
