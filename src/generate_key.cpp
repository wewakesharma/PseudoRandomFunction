#include <random>
//#include <chrono>

//This function will create the input vector randomly and store that value into the array named input.
void generate_rand_key(uint64_t key[4][256], std::mt19937 &generator)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            key[i][j] = generator();
        }
    }
}

int main()
{
    uint64_t key[4][256];

    unsigned seed = 7;            // std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed); // mt19937 is a standard mersenne_twister_engine

    generate_rand_key(key, generator);
}
