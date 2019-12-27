#include <iostream>

struct lastInstCounter
{
    int num_construct {0};
    int num_copy {0};
    int num_move {0};
    int num_destruct {0};
};

static struct lastInstCounter s_lastCounter;


template <typename T>
struct instance_counter {

    instance_counter() noexcept 
    { ++icounter.num_construct; }
    
    instance_counter(const instance_counter&) noexcept 
    { ++icounter.num_copy; }
    
    instance_counter(instance_counter&&) noexcept 
    { ++icounter.num_move; }
    // Simulate both copy-assign and move-assign
    instance_counter& operator=(instance_counter) noexcept
    { return *this; }

    ~instance_counter() 
    { 
        icounter.num_destruct++; 
        s_lastCounter.num_construct = icounter.num_construct;
        s_lastCounter.num_copy = icounter.num_copy;
        s_lastCounter.num_move = icounter.num_move;
        s_lastCounter.num_destruct = icounter.num_destruct;
    }

    static void clear() 
    {
        icounter.num_construct = 0;
        icounter.num_copy = 0;
        icounter.num_move = 0;
        icounter.num_destruct = 0;
    }


private:
    static struct counter 
    {
        int num_construct = 0;
        int num_copy = 0;
        int num_move = 0;
        int num_destruct = 0;

        ~counter()
        {
            s_lastCounter.num_construct = num_construct;
            s_lastCounter.num_copy = num_copy;
            s_lastCounter.num_move = num_move;
            s_lastCounter.num_destruct = num_destruct;

            std::cout << num_construct << " direct constructions" << std::endl;
            std::cout << num_copy << " copies" << std::endl; 
            std::cout << num_move << " moves" << std::endl;
            const int total_construct = num_construct + num_copy + num_move;
            std::cout << total_construct << " total constructions" << std::endl;
            std::cout << num_destruct << " destructions" << std::endl;
            if (total_construct == num_destruct) {
                std::cout << "(no leaks)" << std::endl;
            } else {
                std::cout << "WARNING: potential leak" << std::endl;
            }
        }
    } icounter;
};

template <typename T>
typename instance_counter<T>::counter instance_counter<T>::icounter{};

template <typename T>
struct counted : T, private instance_counter<T>
{
    using T::T;
};