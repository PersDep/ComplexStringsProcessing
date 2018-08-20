#include <ctime>
#include <iostream>
#include <fstream>
#include <random>
#include <string>

using namespace std;

int GetRand(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

char GetRandomChar()
{
    return char(GetRand('A', 'Z'));
}

string GenerateRandomString(int min, int max, int max_level, int max_opened_brackets)
{
    int length = GetRand(min, max);
    int level = 0, counter = 0, i = 0;
    char start_bracket = '(', end_bracket = ')', delimiter = '+';
    bool may_open_brackets = true, opened_bracket = true;
    string result; result += start_bracket;
    while (i++ < length || level != 0)
    {
        if (counter > max_opened_brackets)
            may_open_brackets = false;
        if (GetRand(0, 2)) {
            result += GetRandomChar();
            if (GetRand(0, 1) && level > 0 && !opened_bracket) {
                result += end_bracket;
                level--;
            }
            result += delimiter;
            opened_bracket = false;
        } else if (level < max_level && may_open_brackets) {
            result += start_bracket;
            counter++;
            level++;
            opened_bracket = true;
        }
    }
    result.back() = end_bracket;
    return result;
}

int main(int argc, char **argv)
{
    ofstream file(argv[1]);
    int amount = stoi(argv[2]);
    srand(unsigned(time(nullptr)));
    file << amount << endl;
    for (int i = 0; i < amount; i++)
        file << GenerateRandomString(10, 20, 7, 10) << endl;
    file.close();

    return 0;
}