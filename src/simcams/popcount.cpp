#include <cstdint>
#include <bit>

extern "C" uint8_t
popcount_u8(uint8_t value)
{
    // requires c++20. This gives portable access to the right popcount
    // instruction. Alternatively, could replace with intrinsics for different
    // compilers.
    return std::popcount(value);
}
