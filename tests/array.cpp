#include "Array.h"
#include "output.h"

using namespace man;

int main() {
    int a1[] = { 1, 2, 3 };
    Static<Array<int>> a = a1;

    int a2[] = { 4, 5, 6 };
    Static<Array<int>> b = a2;

    Array<int> c = a + b;
    Array<int> d(c);
    for (auto& i : d)
    {
        P(OUT) << i;
    }
}

