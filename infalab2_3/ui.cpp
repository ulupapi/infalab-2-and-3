#include <iostream>
#include <string>
#include <cassert>
#include <chrono>

#include "DynamicArray.hpp"
#include "LinkedList.hpp"
#include "Sequence.hpp"
#include "MutableArraySequence.hpp"
#include "ImmutableArraySequence.hpp"
#include "MutableListSequence.hpp"
#include "ImmutableListSequence.hpp"
#include "algorithms.hpp"
#include "Queue.hpp"

// ───────────────────────────────  декларации  ──────────────────────────────
void runLab2Tests();
void demoLab2();
void runLab3Tests();
void demoLab3();
void benchLab3();
// ────────────────────────────────────────────────────────────────────────────

int main() {
    while (true) {
        std::cout << "\n=== ЛР 2 и 3 — Главное меню ===\n"
                  << "1) Запустить тесты ЛР 2\n"
                  << "2) Демонстрация алгоритмов ЛР 2\n"
                  << "3) Запустить тесты ЛР 3\n"
                  << "4) Демонстрация очереди ЛР 3\n"
                  << "5) Бенчмарк Append (Массив/Список/Очередь)\n"
                  << "0) Выход\n"
                  << "Выберите> ";
        int c;
        if (!(std::cin >> c)) break;
        switch (c) {
            case 1: runLab2Tests();  break;
            case 2: demoLab2();      break;
            case 3: runLab3Tests();  break;
            case 4: demoLab3();      break;
            case 5: benchLab3();     break;
            case 0: std::cout << "До свидания!\n"; return 0;
            default: std::cout << "Некорректный выбор\n";
        }
    }
    return 0;
}

// ───────────────────────────────  ЛАБА 2  ──────────────────────────────────
void runLab2Tests() {
    std::cout << "\n-- Тесты ЛР 2 --\n";

    int init[] = {1,2,3};
    const ImmutableArraySequence<int> src(init, 3);

    // immutable Append → SeqUPtr
    auto s1 = src.Append(4);
    assert(s1->GetLength() == 4 && s1->GetLast() == 4);

    // Map, FlatMap, Where
    auto map  = Map<int,int>(*s1, [](int x){ return x*2; });
    auto flat = FlatMap<int,int>(*s1, [](int x){
        auto seq = std::make_unique<MutableArraySequence<int>>();
        seq->Append(1); seq->Append(x);
        return seq;
    });
    assert(flat->GetLength() == 8);

    auto even = Where<int>(*map, [](int x){ return x % 4 == 0; });
    assert(even->GetLength() == 2);

    // Reduce   2+4+6+8 = 20
    int sum = Reduce<int,int>(*map, 0, [](int s,int v){ return s + v; });
    assert(sum == 20);

    // Find / TryFind
    assert(Find<int>(*map, [](int v){ return v == 6; }) == 6);
    int out; bool ok = TryFind<int>(*map, [](int v){ return v == 7; }, out);
    assert(!ok);

    // Zip / Unzip
    auto pairs = Zip<int,int>(*s1, *map);
    auto [u1, u2] = Unzip<int,int>(*pairs);
    assert(u1->GetLength() == u2->GetLength());

    // ContainsSubsequence
    int patArr[] = {2,3};
    MutableArraySequence<int> pat(patArr, 2);
    assert(ContainsSubsequence<int>(*s1, pat));

    // Split / Slice
    auto chunks = Split<int>(*s1, [](int x){ return x % 2 == 0; });
    assert(chunks->GetLength() >= 1);

    auto sliced = Slice<int>(*s1, 1, 2);
    assert(sliced->GetLength() == 2 && sliced->Get(0) == 1 && sliced->Get(1) == 4);

    std::cout << "Тесты ЛР 2 (массив) пройдены!\n";

    // ───────────────────────────────  Тесты ListSequence  ──────────────────────────────

    // 1) MutableListSequence<int>
    {
        int initListArr[] = {10, 20, 30, 40, 50};
        MutableListSequence<int> mutableSeq(initListArr, 5);
        assert(mutableSeq.GetLength() == 5);

        // Подпоследовательность [1..3] → {20, 30, 40}
        auto subMutable = mutableSeq.GetSubsequence(1, 3);
        assert(subMutable->GetLength() == 3);
        assert(subMutable->Get(0) == 20);
        assert(subMutable->Get(1) == 30);
        assert(subMutable->Get(2) == 40);

        // Оригинал не изменился
        assert(mutableSeq.GetLength() == 5);
        assert(mutableSeq.Get(0) == 10 && mutableSeq.Get(4) == 50);

        // Изменяем подпоследовательность (мутабельно)
        static_cast<MutableListSequence<int>*>(subMutable.get())->Append(60);
        assert(subMutable->GetLength() == 4);
        assert(subMutable->Get(3) == 60);
    }

    // 2) ImmutableListSequence<int>
    {
        int initImmArr[] = {1, 2, 3, 4, 5, 6};
        const ImmutableListSequence<int> immSeq(initImmArr, 6);
        assert(immSeq.GetLength() == 6);

        // Подпоследовательность [2..4] → {3, 4, 5}
        auto subImm = immSeq.GetSubsequence(2, 4);
        assert(subImm->GetLength() == 3);
        assert(subImm->Get(0) == 3);
        assert(subImm->Get(1) == 4);
        assert(subImm->Get(2) == 5);

        // Оригинал не изменился
        assert(immSeq.GetLength() == 6);
        assert(immSeq.GetFirst() == 1 && immSeq.GetLast() == 6);

        // Попытка изменить подпоследовательность (immutable) → exception
        bool caught = false;
        try {
            static_cast<ImmutableListSequence<int>*>(subImm.get())->Append(7);
        } catch (const std::logic_error&) {
            caught = true;
        }
        assert(caught);
    }

    std::cout << "Тесты ListSequence пройдены!\n";
}

// ───────────────────────────────  ЛАБА 2  Продолжение (Demo) ──────────────────────────────────
void demoLab2() {
    std::cout << "\n-- Демонстрация ЛР 2 --\n";
    int arr[] = {1,2,3,4,5};
    const ImmutableArraySequence<int> seq(arr, 5);

    std::cout << "Оригинал: ";
    for (int x : seq) std::cout << x << ' ';
    std::cout << "\n";

    auto mapped = Map<int,int>(seq, [](int x){ return x*10; });
    std::cout << "Map (*10): ";
    for (int x : *mapped) std::cout << x << ' ';
    std::cout << "\n";

    auto filtered = Where<int>(*mapped, [](int x){ return x > 20; });
    std::cout << "Where (>20): ";
    for (int x : *filtered) std::cout << x << ' ';
    std::cout << "\n";

    std::cout << "Zip (orig & mapped): ";
    auto z = Zip<int,int>(seq, *mapped);
    for (std::size_t i = 0; i < z->GetLength(); ++i) {
        auto p = z->Get(i);
        std::cout << '(' << p.first << ',' << p.second << ") ";
    }
    std::cout << "\nUnzip A: ";
    auto [a, b] = Unzip<int,int>(*z);
    for (int x : *a) std::cout << x << ' ';
    std::cout << "\n";
}

// ───────────────────────────────  ЛАБА 3  ──────────────────────────────────
void runLab3Tests() {
    std::cout << "\n-- Тесты ЛР 3 --\n";

    QueueSequence<int> q;
    q.Enqueue(10); q.Enqueue(20); q.Enqueue(30);
    assert(q.GetLength() == 3 && q.Peek() == 10);

    int d = q.Dequeue();
    assert(d == 10 && q.Peek() == 20);

    // Partition
    auto [big, small] = PartitionQueue<int>(q, [](int x){ return x >= 25; });
    assert(big->GetLength() == 1 && small->GetLength() == 1);

    // Исключения при пустой очереди
    q.Dequeue(); q.Dequeue();  // теперь очередь пуста
    bool caught = false;
    try { q.Peek(); } catch (...) { caught = true; }
    assert(caught);

    std::cout << "Тесты ЛР 3 пройдены!\n";
}

void demoLab3() {
    std::cout << "\n-- Демонстрация ЛР 3 --\n";
    QueueSequence<std::string> q;
    q.Enqueue("alice"); q.Enqueue("bob"); q.Enqueue("carol");

    std::cout << "Содержимое очереди: ";
    for (std::size_t i = 0; i < q.GetLength(); ++i)
        std::cout << q.Get(i) << ' ';
    std::cout << "\nPeek: " << q.Peek() << "\n";

    // Partition по первому символу 'a'
    auto [aQ, otherQ] = PartitionQueue<std::string>(q, [](const std::string& s){
        return !s.empty() && s[0] == 'a';
    });
    std::cout << "Partition YES: ";
    for (std::size_t i = 0; i < aQ->GetLength(); ++i)
        std::cout << aQ->Get(i) << ' ';
    std::cout << "\nPartition NO: ";
    for (std::size_t i = 0; i < otherQ->GetLength(); ++i)
        std::cout << otherQ->Get(i) << ' ';
    std::cout << "\n";

    std::cout << "Dequeuing all: ";
    while (q.GetLength())
        std::cout << q.Dequeue() << ' ';
    std::cout << "\n";
}

void benchLab3() {
    std::cout << "\n-- Бенчмарк Append (100 000) --\n";
    const std::size_t N = 100000;
    using Clock = std::chrono::high_resolution_clock;

    auto bench = [&](auto label, auto appendFn){
        auto t0 = Clock::now();
        appendFn();
        auto t1 = Clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        std::cout << label << ": " << ms << " ms\n";
    };

    bench("MutableArraySequence", [&]{
        MutableArraySequence<int> s;
        for (std::size_t i = 0; i < N; ++i) s.Append((int)i);
    });
    bench("MutableListSequence", [&]{
        MutableListSequence<int> s;
        for (std::size_t i = 0; i < N; ++i) s.Append((int)i);
    });
    bench("QueueSequence (list-based)", [&]{
        QueueSequence<int> q2;
        for (std::size_t i = 0; i < N; ++i) q2.Enqueue((int)i);
    });
    bench("ImmutableArraySequence", [&]{
        auto p = std::make_unique<ImmutableArraySequence<int>>();
        for (std::size_t i = 0; i < N; ++i) p->Append((int)i);
    });
}
