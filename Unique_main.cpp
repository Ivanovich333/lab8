#include <iostream>
#include <utility> // Для std::move

// Шаблон класса MyUnique
template<class T>
class MyUnique
{
    T* p = nullptr;

    // Запрет копирования
    MyUnique(const MyUnique&) = delete;
    MyUnique& operator=(const MyUnique&) = delete;

public:
    // Конструктор, принимающий сырой указатель
    explicit MyUnique(T* p = nullptr) : p(p) {}

    // Деструктор
    ~MyUnique() {
        delete p;
    }

    // Конструктор перемещения
    MyUnique(MyUnique&& other) noexcept : p(other.p) {
        other.p = nullptr;
    }

    // Оператор присваивания с перемещением
    MyUnique& operator=(MyUnique&& other) noexcept {
        if (this != &other) {
            delete p;
            p = other.p;
            other.p = nullptr;
        }
        return *this;
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

    // Явное освобождение ресурса
    void reset(T* ptr = nullptr) {
        delete p;
        p = ptr;
    }

    // Возвращает указатель и освобождает владение
    T* release() {
        T* oldPtr = p;
        p = nullptr;
        return oldPtr;
    }
};

// Функция Make_MyUnique
template<class T, class... Args>
MyUnique<T> Make_MyUnique(Args&&... args) {
    return MyUnique<T>(new T(std::forward<Args>(args)...));
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
    // Создание уникального указателя с помощью Make_MyUnique
    MyUnique<MyPoint> ptr = Make_MyUnique<MyPoint>(10, 20);
    ptr->print();  // Point(10, 20)

    // Перемещение указателя
    MyUnique<MyPoint> ptr2 = std::move(ptr);
    if (!ptr.get()) {
        std::cout << "ptr is now null\n";  // ptr is now null
    }
    ptr2->print();  // Point(10, 20)

    // Сброс указателя
    ptr2.reset();
    if (!ptr2.get()) {
        std::cout << "ptr2 is now null\n";  // ptr2 is now null
    }

    // Пример использования reset с новым указателем
    ptr2.reset(new MyPoint(30, 40));
    ptr2->print();  // Point(30, 40)

    // Пример использования release
    MyPoint* rawPtr = ptr2.release();
    if (!ptr2.get()) {
        std::cout << "ptr2 is now null after release\n";  // ptr2 is now null after release
    }
    delete rawPtr;  // Не забываем очистить память, если используем release

    return 0;
}
