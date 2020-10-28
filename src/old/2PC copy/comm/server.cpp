#include <iostream>

#include "recv.h"
#include "snd.h"

using namespace std;
//Server side
int main()
{
    pi_recv();
    pi_snd("hello");
    return 0;   
}