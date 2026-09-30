#include <iostream>

using std::string;
using std::getline, std::cin, std::cout, std::endl;

int main()
{
    int n;
    char c;

    string str = "";

    cout << "Enter N: ";
    cin >> n;

    cout << "Enter c: ";
    cin >> c;

    cout << "String is " << string(n, c) << endl;

    return 0;
}
