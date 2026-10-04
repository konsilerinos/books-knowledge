#set page(
  paper: "a4",
  margin: (x: 1cm, y: 1cm),
  // height: auto
)

// Ссылка на ресурс:
// <a href="https://konsilerinos.github.io/books-knowledge/extra/cpp/tasks-0-100.html#task-2">see extra</a>

= 0. Hello-world <task-0>
```cpp
#include <iostream>

int main()
{
  std::cout << "Hello, World!" << std::endl;
  return 0;
}
```

= 1. До тех пор пока не введена строка "stop" вводить пару строк и сравнивать их <task-1>
```cpp
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
```

= 2. На входе int n, char c, на выходе - string из n символов c <task-2>
```cpp
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
```

= 3. До тех пор пока не введена строка "stop" читать строки из cin потока и выводить их в cout поток. Если строка пустая, то вывести "empty" <task-3>
```cpp
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

        if (str.empty())
        {
            cout << "empty" << endl;
        }
        else
        {
            cout << str << endl;
        }
    }

    return 0;
}
```

= 4. До тех пор пока не введена строка "stop" читать строки из cin потока и выводить их размер в cout поток <task-4>
```cpp
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
```
