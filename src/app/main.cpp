#include <iostream>
#include <memory>
using namespace std;


int main() {
    int a = 10;
    int b;
    int& refA = a;
    cout << "Address of a: " << &a << endl;
    cout << "Address of refA: " << &refA << endl;
}