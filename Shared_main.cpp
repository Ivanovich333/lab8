#include <iostream>
#include <utility> // Для std::move

// Шаблон класса MyShared
template<class T>
class MyShared
{
    T* p = nullptr;
    int* ref_count = nullptr;

    void release() {
        if (ref_count) {
            --(*ref_count);
            if (*ref_count == 0) {
                delete p;
                delete ref_count;
            }
        }
    }

public:
    // Конструктор, принимающий сырой указатель
    explicit MyShared(T* ptr = nullptr) : p(ptr), ref_count(new int(1)) {}

    // Конструктор копирования
    MyShared(const MyShared& other) : p(other.p), ref_count(other.ref_count) {
        if (ref_count) {
            ++(*ref_count);
        }
    }

    // Оператор присваивания с копированием
    MyShared& operator=(const MyShared& other) {
        if (this != &other) {
            release();
            p = other.p;
            ref_count = other.ref_count;
            if (ref_count) {
                ++(*ref_count);
            }
        }
        return *this;
    }

    // Конструктор перемещения
    MyShared(MyShared&& other) noexcept : p(other.p), ref_count(other.ref_count) {
        other.p = nullptr;
        other.ref_count = nullptr;
    }

    // Оператор присваивания с перемещением
    MyShared& operator=(MyShared&& other) noexcept {
        if (this != &other) {
            release();
            p = other.p;
            ref_count = other.ref_count;
            other.p = nullptr;
            other.ref_count = nullptr;
        }
        return *this;
    }

    // Деструктор
    ~MyShared() {
        release();
    }

    // Метод get
    T* get() const {
        return p;
    }

    // Перегрузка оператора *
    T& operator*() const {
        return *p;
    }

    // Перегрузка оператора ->
    T* operator->() const {
        return p;
    }

    // Получение текущего значения счетчика ссылок
    int use_count() const {
        return ref_count ? *ref_count : 0;
    }
};

// Функция Make_MyShared
template<class T, class... Args>
MyShared<T> Make_MyShared(Args&&... args) {
    return MyShared<T>(new T(std::forward<Args>(args)...));
}

// Пример класса MyPoint
class MyPoint {
    int x, y;
public:
    MyPoint(int x, int y) : x(x), y(y) {}
    void print() const {
        std::cout << "Point(" << x << ", " << y << ")\n";
    }
};

// Функция main для демонстрации всех возможностей
int main() {
    // Создание shared указателя с помощью Make_MyShared
    MyShared<MyPoint> ptr1 = Make_MyShared<MyPoint>(10, 20);
    ptr1->print();  // Point(10, 20)
    std::cout << "ptr1 use count: " << ptr1.use_count() << "\n";  // ptr1 use count: 1

    // Копирование shared указателя
    MyShared<MyPoint> ptr2 = ptr1;
    ptr2->print();  // Point(10, 20)
    std::cout << "ptr1 use count: " << ptr1.use_count() << "\n";  // ptr1 use count: 2
    std::cout << "ptr2 use count: " << ptr2.use_count() << "\n";  // ptr2 use count: 2

    // Перемещение shared указателя
    MyShared<MyPoint> ptr3 = std::move(ptr2);
    ptr3->print();  // Point(10, 20)
    std::cout << "ptr1 use count: " << ptr1.use_count() << "\n";  // ptr1 use count: 2
    std::cout << "ptr2 use count: " << ptr2.use_count() << "\n";  // ptr2 use count: 0
    std::cout << "ptr3 use count: " << ptr3.use_count() << "\n";  // ptr3 use count: 2

    return 0;
}
