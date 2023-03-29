#include <fstream>
#include <string>
#include <iostream>

int main()
{
    std::fstream fs;
    fs.open("test.txt", std::ios::out);
    for (int i = 1; i < 61; ++i)
    {
        //for each speaker
        for (int j = 1; j < 6; ++j)
        {
            if (j == 1)
            {
                fs << "<s> QI </s> ";
            }
            if (j == 2)
            {
                fs << "<s> RAN </s> ";
            }
            if (j == 3)
            {
                fs << "<s> SHOU </s> ";
            }
            if (j == 4)
            {
                fs << "<s> TUI </s> ";
            }
            if (j == 5)
            {
                fs << "<s> DING </s> ";
            }
            fs << "(" << i << "_0" << j << ")\n";
        }
    }
    if (!fs) std::cout << "file not creted\n";
    fs.close();
    std::cout << "test!\n";
    return 0;
}