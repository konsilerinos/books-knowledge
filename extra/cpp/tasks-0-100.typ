#set page(paper: "a4", margin: (x: 1cm, y: 1cm))

#show heading: it => {
  if it.has("label") {
    [#link(it.label)[]] + it
  } else {
    it
  }
}

= Task-0 <task-0>
== main.cpp

```cpp
#include <iostream>

int main()
{
  std::cout << "Hello, World!" << std::endl;
  return 0;
}
```

= Task-1 <task-1>
== main.cpp

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

= Task-2 <task-2>
== main.cpp

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

= Tak-3 <task-3>
== main.cpp

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

= Task-4 <task-4>
== main.cpp

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
