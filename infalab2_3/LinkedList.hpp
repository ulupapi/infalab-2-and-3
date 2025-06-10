#pragma once

#include <stdexcept>
#include <initializer_list>
#include <cstddef>

// Шаблон односвязного списка
template<typename T>
class LinkedList {
public:
    // --- Вложенная структура узла ---
    struct Node {
        T val;       // хранимое значение
        Node* next;  // указатель на следующий узел
        explicit Node(const T& v) : val(v), next(nullptr) {}
    };

private:
    Node* head_{nullptr};      // указатель на первый узел
    Node* tail_{nullptr};      // указатель на последний узел
    std::size_t len_{0};       // текущее число элементов

    // Утилитный метод очистки списка
    void clear() {
        Node* cur = head_;
        while (cur) {
            Node* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        head_ = tail_ = nullptr;
        len_ = 0;
    }

public:
    // --- Конструкторы / деструктор ---
    LinkedList() = default;

    // Конструктор из массива: добавляем count элементов
    LinkedList(const T* items, std::size_t count) {
        for (std::size_t i = 0; i < count; ++i)
            Append(items[i]);
    }

    // Конструктор из initializer_list (например: {a, b, c})
    LinkedList(const std::initializer_list<T>& init) {
        for (const T& v : init)
            Append(v);
    }

    // Конструктор копирования: создаём новый список, копируя каждый узел
    LinkedList(const LinkedList& other) : LinkedList() {
        Node* cur = other.head_;
        while (cur) {
            Append(cur->val);
            cur = cur->next;
        }
    }

    // Оператор присваивания копированием
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            clear();
            Node* cur = other.head_;
            while (cur) {
                Append(cur->val);
                cur = cur->next;
            }
        }
        return *this;
    }

    ~LinkedList() {
        clear();
    }

    // --- Доступ к данным ---
    std::size_t GetLength() const {
        return len_;
    }

    const T& Get(std::size_t idx) const {
        if (idx >= len_)
            throw std::out_of_range("LinkedList::Get: bad index");
        Node* cur = head_;
        for (std::size_t i = 0; i < idx; ++i)
            cur = cur->next;
        return cur->val;
    }

    T GetFirst() const {
        if (!head_)
            throw std::out_of_range("LinkedList::GetFirst: empty");
        return head_->val;
    }

    T GetLast() const {
        if (!tail_)
            throw std::out_of_range("LinkedList::GetLast: empty");
        return tail_->val;
    }

    // --- Модификаторы ---
    void Append(const T& v) {
        Node* n = new Node(v);
        if (!head_) {
            head_ = tail_ = n;
        } else {
            tail_->next = n;
            tail_ = n;
        }
        ++len_;
    }

    void Prepend(const T& v) {
        Node* n = new Node(v);
        n->next = head_;
        head_ = n;
        if (!tail_)
            tail_ = n;
        ++len_;
    }

    void InsertAt(const T& v, std::size_t idx) {
        if (idx > len_)
            throw std::out_of_range("LinkedList::InsertAt: bad idx");
        if (idx == 0) {
            Prepend(v);
            return;
        }
        if (idx == len_) {
            Append(v);
            return;
        }
        Node* cur = head_;
        for (std::size_t i = 1; i < idx; ++i)
            cur = cur->next;
        Node* n = new Node(v);
        n->next = cur->next;
        cur->next = n;
        ++len_;
    }

    // Конкатенация: создаём новый список, в котором сначала копируем *this,
    // затем добавляем все элементы other
    LinkedList* Concat(const LinkedList* other) const {
        LinkedList* out = new LinkedList(*this);
        Node* cur = other->head_;
        while (cur) {
            out->Append(cur->val);
            cur = cur->next;
        }
        return out;
    }

    // --- Операторы индексации ---
    T& operator[](std::size_t idx) {
        if (idx >= len_)
            throw std::out_of_range("LinkedList::operator[]: bad index");
        Node* cur = head_;
        for (std::size_t i = 0; i < idx; ++i)
            cur = cur->next;
        return cur->val;
    }

    const T& operator[](std::size_t idx) const {
        return Get(idx);
    }

    // --- Геттер для head_ (нужен в ListSequence) ---
    Node* getHead() const {
        return head_;
    }
};
