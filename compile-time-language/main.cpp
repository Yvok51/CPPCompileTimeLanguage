
#include <iostream>
#include "meta_asm.hpp"


int main()
{
    /*
    constexpr size_t a = 1;
    constexpr size_t b = 0;
    constexpr size_t c = 8;
    constexpr size_t d = 5;
    constexpr size_t z = 10000;
    
    constexpr int val = static_list_get_id_v<
        b,
        variable_list_set_value_t<
            b,
            20,
            static_list<
                variable<a, 1>,
                variable<b, 2>,
                variable<c, 3>,
                variable<d, 4>
            >
        >
    >;
    
    constexpr int n = execute_v <
        variable<a, 1>,
        variable<b, 2>,
        variable<c, 3>,
        variable<d, 4>,
        jump_if_not_zero<a, 8>,
        store<b, c>,
        add<b, b>,
        ret<b>
    >;
    std::cout << n << std::endl;
    std::cout << val << std::endl;
    */
    
    constexpr size_t PREV = 0;
    constexpr size_t CURR = 1;
    constexpr size_t OLD_CURR = 2;
    constexpr size_t N = 3;
    constexpr size_t MINUS1 = 4;
    constexpr size_t NON_EXISTENT = 100;

    constexpr int fib_n = execute_v<
        variable<PREV, 0>,              // 1: PREV = 0
        variable<CURR, 1>,              // 2: CURR = 1
        variable<OLD_CURR, 0>,          // 3: OLD_CURR = 0
        variable<N, 10>,                // 4: N = 10
        variable<MINUS1, -1>,           // 5: MINUS1 = -1

        jump_if_not_zero<N, 8>,         // 6: if (N > 0) goto 8
        ret<PREV>,                      // 7: return 0

        store<OLD_CURR, CURR>,          // 8: OLD_CURR = CURR
        add<CURR, PREV>,                // 9: CURR += PREV
        store<PREV, OLD_CURR>,          // 10: PREV = OLD_CURR
        add<N, MINUS1>,                 // 11: N += -1
        jump_if_not_zero<N, 8>,         // 12: if (N > 0) goto 8
        ret<PREV>                       // 13: return PREV
    >;
    // static_assert(fib_n == 55, "Wrong result");
    std::cout << fib_n << std::endl;
}