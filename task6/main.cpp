#include <iostream>
#include "LazyString.h"

using namespace std;

int main() {
	string ls("HelloWorld");
    cout << ls << endl;
    cout << ls.substr(5, 5);
    return 0;
};
