#include <iostream>

using std::string;
using std::getline, std::cin, std::cout, std::endl;

int main()
{
    string str;

    while(getline(cin, str))
    {
        if (str == "stop")
        {
            break;
        }

        cout << str.size() << endl;
    }

    return 0;
}
