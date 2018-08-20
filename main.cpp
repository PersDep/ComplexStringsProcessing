#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

using namespace std;

typedef pair<string, string> TwoStrings;

class Sorter
{
    vector<TwoStrings> sortable_elems;
    size_t entry_point;
    bool reading, nested_string;

    static bool cmp(const TwoStrings &a, const TwoStrings &b) { return a.second < b.second; }

public:
    Sorter() : entry_point(0), reading(true), nested_string(false) {}

    void FillSortableElems(map<int, vector<TwoStrings>>::reverse_iterator it, int &inserts_counter,
                           const string &str, size_t end_point)
    {
        if (nested_string) {
            auto temp_it = --it;
            sortable_elems.emplace_back(TwoStrings((temp_it)->second[inserts_counter].first,
                                                   (temp_it)->second[inserts_counter].second));
            it++;
            inserts_counter++;
            nested_string = false;
        } else {
            sortable_elems.emplace_back(TwoStrings(str.substr(entry_point + 1, end_point - entry_point - 1),
                                                   str.substr(entry_point + 1, end_point - entry_point - 1)));
        }
    }

    void Sort(TwoStrings &strs)
    {
        sort(sortable_elems.begin(), sortable_elems.end(), cmp);
        strs.first = "(";
        for (auto &str_parts : sortable_elems) {
            strs.first += str_parts.first + '+';
            strs.second += str_parts.second;
        }
        strs.first[strs.first.size() - 1] = ')';
    }

    void SaveEntryPoint(size_t entry_point) { this->entry_point = entry_point; }

    void AllowReading() { reading = true; }
    void BlockReading() { reading = false; nested_string = true; }
    bool Reading() { return reading; }
};

class NestringController
{
    int nesting_level;
    const char start_bracket = '(', end_bracket = ')';

public:
    NestringController() : nesting_level(0) {}

    void Clear() { nesting_level = 0; }

    int Shift(char cur)
    {
        if (cur == start_bracket) {
            nesting_level++;
            return 1;
        }
        if (cur == end_bracket) {
            nesting_level--;
            if (!nesting_level)
                return -2;
            return -1;
        }
        return 0;
    }

    bool IsGoingToIncrease(char c) { return c == start_bracket; }

    int GetNestingLevel() { return nesting_level; }
};

class ExpressionsMap
{
    map<int, vector<TwoStrings>> elems;
    vector<size_t> entry_points;
    NestringController nesting_level;
    const char delimiter = '+';

    void saveEntryPoint(size_t entry_point)
    {
        if (entry_points.size() < nesting_level.GetNestingLevel())
            entry_points.emplace_back(entry_point);
        else
            entry_points[nesting_level.GetNestingLevel() - 1] = entry_point;
    }

public:
    explicit ExpressionsMap(const string &input)
    {
        for (size_t i = 0; i < input.size(); i++) {
            int nesting_level_shift = nesting_level.Shift(input[i]);
            if (nesting_level_shift > 0)
                saveEntryPoint(i);
            if (nesting_level_shift < 0)
                elems[nesting_level.GetNestingLevel()].emplace_back(TwoStrings(input.substr(
                        entry_points[nesting_level.GetNestingLevel()],
                        i - entry_points[nesting_level.GetNestingLevel()] + 1), string()));
        }
    }

    string Sort()
    {
        for (auto it = elems.rbegin(); it != elems.rend(); it++) {
            int inserts_counter = 0;
            for (auto &strs : it->second) {
                Sorter sorter; nesting_level.Clear();
                for (size_t i = 1; i < strs.first.size() - 1; i++) {
                    if (strs.first[i] == delimiter && sorter.Reading()) {
                        sorter.FillSortableElems(it, inserts_counter, strs.first, i);
                        sorter.SaveEntryPoint(i);
                        if (nesting_level.IsGoingToIncrease(strs.first[i + 1]))
                            sorter.BlockReading();
                    }
                    int nesting_level_shift = nesting_level.Shift(strs.first[i]);
                    if (nesting_level_shift > 0 && sorter.Reading()) {
                        sorter.SaveEntryPoint(i - 1);
                        sorter.BlockReading();
                    }
                    if (nesting_level_shift == -2)
                        sorter.AllowReading();
                }
                sorter.FillSortableElems(it, inserts_counter, strs.first, strs.first.size() - 1);
                sorter.Sort(strs);
            }
        }
        return elems[0][0].first;
    }

    friend ostream& operator<<(ostream& os, const ExpressionsMap& stringMap);
};

ostream& operator<<(ostream& os, const ExpressionsMap& stringMap)
{
    for (auto &level : stringMap.elems) {
        for (auto &elem : level.second)
            os << elem.first << ' ' << elem.second << ' ';
        os << endl;
    }
    return os;
}

string A(const string &input)
{
    ExpressionsMap expressionsMap(input);

//    cout << expressionsMap << endl;

    string result = expressionsMap.Sort();

//    cout << expressionsMap << endl;

    return result;
}

int main(int argc, char **argv)
{
    int amount;
    ifstream stringsFile(argv[1]);
    ofstream sortedStringsFile(string(argv[1]) + string("_sorted"));
    stringsFile >> amount;
    for (int i = 0; i < amount; i++) {
        string buf;
        stringsFile >> buf;
        sortedStringsFile << A(buf) << endl;
    }
    sortedStringsFile.close();
    stringsFile.close();

    return 0;
}