#include <fstream>
#include <string>
#include <iostream>

int main()
{
    std::fstream fs;
    fs.open("transcriptiontest.txt", std::ios::out);
    for (int i = 1; i < 196 ; ++i)  //1-90 91-145
    {
        //for each speaker
        for (int j=1; j < 8 ; ++j)
        {
            if (i < 91 && j < 4)
            {
                if (j == 1)
                {
                    fs << "<s> JIANBURUFEI </s> ";
                }
                if (j == 2)
                {
                    fs << "<s> HUNSHUIMOYU </s> ";
                }
                if (j == 3)
                {
                    fs << "<s> HUOMAOSANZHANG </s> ";
                }
                fs << "(" << i << "_0" << j << ")\n";
            }
            if (i > 90 && i < 146 && j > 3 && j < 7)
            {
                if (j == 4)
                {
                    fs << "<s> JINGANGBUHUAI </s> ";
                }
                if (j == 5)
                {
                    fs << "<s> YAQUEWUSHENG </s> ";
                }
                if (j == 6)
                {
                    fs << "<s> ZHIMAKAIMEN </s> ";
                }
                fs << "(" << i << "_0" << j << ")\n";
            }
            if (i > 145 && j > 6)
            {
                if (j == 7)
                {
                    fs << "<s> GONGXIFACAI </s> ";
                }
                fs << "(" << i << "_0" << j << ")\n";
            }

        }

    }
    if (!fs) std::cout << "file not creted\n";
    fs.close();

    std::fstream nfs;
    nfs.open("fileidtest.txt", std::ios::out);
    for (int i = 1; i < 196 ; ++i)
    {
        //for each speaker
        //speaker_i/i_0j
        for (int j=1; j < 8 ; ++j)
        {
            if (i < 91 && j < 4)
            {
                nfs << "speaker_" << i << "/" << i << "_0" << j << "\n";
            }
            if (i > 90 && i < 146 && j > 3 && j < 7)
            {
                nfs << "speaker_" << i << "/" << i << "_0" << j << "\n";
            }
            if (i > 145 && j > 6)
            {
                nfs << "speaker_" << i << "/" << i << "_0" << j << "\n";
            }
            
        }
    }
    if (!nfs) std::cout << "file not creted\n";
    nfs.close();
    std::cout << "test!\n";
    return 0;
}