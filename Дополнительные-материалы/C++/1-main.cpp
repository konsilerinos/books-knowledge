#include <iostream>

using std::string;
using std::getline, std::cin, std::cout, std::endl;

int main()
{
    string str1, str2, state;
    cout << "Compare strings" << endl;
    
    while(true)
    {
        if (str1 == "stop")
        {
            break;
        }

        cout << "First: ";
        getline(cin, str1);

        cout << "Second: ";
        getline(cin, str2);

        if (str1 < str2)
        {
            state = "<";
        }
        else if (str1 > str2)
        {
            state = ">";
        }
        else if (str1 == str2)
        {
            state = "==";
        }

        cout << str1 << " " << state << " " << str2 << endl;
    }

    return 0;
}