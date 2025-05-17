// mt19937_clone.cpp
#include <array>
#include <cstdint>
#include <iostream>
#include <random>

class MT19937 {
    public:
    static constexpr int w = 32, n = 624, m = 397, r = 31;
    static constexpr uint32_t a = 0x9908B0DF;
    static constexpr uint32_t d = 0xFFFFFFFF;
    static constexpr uint32_t b = 0x9D2C5680;
    static constexpr uint32_t c = 0xEFC60000;
    static constexpr int u = 11, s = 7, t = 15, l = 18;
    static constexpr uint32_t f = 1812433253u;

    static constexpr uint32_t lower_mask = (1u << r) - 1;
    static constexpr uint32_t upper_mask = (~lower_mask) & 0xFFFFFFFFu;

    MT19937(uint32_t seed = 0x19937u) {
        state.fill(0);
        idx = n;
        initialize(seed);
    }

    // inject recovered state
    void set_state(const std::array<uint32_t,n>& recovered) {
        state = recovered;
        idx = n;
    }

    // standard MT tempering
    uint32_t temper() {
        if (idx >= n) twist();
        uint32_t y = state[idx++];
        y ^= ((y >> u) & d);
        y ^= ((y << s) & b);
        y ^= ((y << t) & c);
        y ^= (y >> l);
        return y;
    }

private:
    std::array<uint32_t,n> state;
    int idx;

    void initialize(uint32_t seed) {
        state[0] = seed;
        for (int i = 1; i < n; ++i) {
            uint32_t prev = state[i-1];
            state[i] = (f * (prev ^ (prev >> (w-2))) + i) & 0xFFFFFFFFu;
        }
        twist();
    }

    void twist() {
        for (int i = 0; i < n; ++i) {
            uint32_t x = (state[i] & upper_mask)
                       + (state[(i+1)%n] & lower_mask);
            uint32_t xA = x >> 1;
            if (x & 1u) xA ^= a;
            state[i] = state[(i+m)%n] ^ xA;
        }
        idx = 0;
    }

public:
    static uint32_t untemper(uint32_t y) {
        y = inv_right(y, l, d);
        y = inv_left (y, t, c);
        y = inv_left (y, s, b);
        y = inv_right(y, u, d);
        return y;
    }

private:
    static uint32_t inv_right(uint32_t y, int shift, uint32_t mask) {
        uint32_t x = 0;
        // must go from the most significant bit down
        for (int i = w-1; i >= 0; --i) {
            uint32_t ybit = (y >> i) & 1u;
            uint32_t mbit = (mask >> i) & 1u;
            uint32_t xbit;
            if (i + shift >= w) {
                // no dependency on higher x-bits
                xbit = ybit;
            } else {
                // xbit ^ ((x>>(i+shift)&1) & mbit) == ybit
                xbit = ybit ^ (((x >> (i + shift)) & 1u) & mbit);
            }
            x |= (xbit << i);
        }
        return x;
    }
    static uint32_t inv_left(uint32_t y, int shift, uint32_t mask) {
        uint32_t x = 0;
        for (int i = 0; i < w; ++i) {
            uint32_t ybit = (y >> i) & 1u;
            uint32_t mbit = (mask >> i) & 1u;
            uint32_t xbit;
            if (i < shift) {
                // no dependency on lower x-bits
                xbit = ybit;
            } else {
                // xbit ^ ((x>>(i-shift)&1) & mbit) == ybit
                xbit = ybit ^ (((x >> (i - shift)) & 1u) & mbit);
            }
            x |= (xbit << i);
        }
        return x;
    }
};

template<class F, class G>
void compare_streams(F f1, G f2, uint32_t lim = 100000) {
    for (size_t i = 0; i < lim; ++i) {
        if (f1() != f2()) {
            std::cout << "Outputs differ at index " << i << std::endl;
            return;
        }
    }
    std::cout << lim << " outputs match" << std::endl;
}

int main() {
    // === Manual MT19937 clone test ===
    MT19937 orig(0);
    MT19937 clone1;
    std::array<uint32_t, MT19937::n> recovered{};
    for (int i = 0; i < MT19937::n; ++i) {
        uint32_t y = orig.temper();
        recovered[i] = MT19937::untemper(y);
    }
    clone1.set_state(recovered);
    std::cout << "=== Manual MT19937 clone ===" << std::endl;
    compare_streams([&]{ return orig.temper(); },
                    [&]{ return clone1.temper(); });

    // === std::mt19937 clone test ===
    std::mt19937 stdrng(0);
    MT19937 clone2;
    for (int i = 0; i < MT19937::n; ++i) {
        uint32_t y = stdrng();
        recovered[i] = MT19937::untemper(y);
    }
    clone2.set_state(recovered);
    std::cout << "\n=== std::mt19937 clone ===" << std::endl;
    compare_streams([&]{ return stdrng(); },
                    [&]{ return clone2.temper(); });

    return 0;
}
