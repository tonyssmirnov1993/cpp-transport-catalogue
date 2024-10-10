#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <functional>
#include <optional>

using namespace std;

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end)
        : first(begin)
        , last(end) {
    }

    Iterator begin() const {
        return first;
    }

    Iterator end() const {
        return last;
    }

private:
    Iterator first, last;
};

template <typename Collection>
auto Head(Collection& v, size_t top) {
    return IteratorRange{ v.begin(), next(v.begin(), min(top, v.size())) };
}

struct Person {
    string name;
    int age, income;
    bool is_male;
};

vector<Person> ReadPeople(istream& input) {
    int count;
    input >> count;

    vector<Person> result(count);
    for (Person& p : result) {
        char gender;
        input >> p.name >> p.age >> p.income >> gender;
        p.is_male = gender == 'M';
    }

    return result;
}
struct CommandResults {
    optional<string> most_popular_male_name;
    optional<string> most_popular_female_name;
    vector<int> cumulative_wealth;
    vector<Person> sorted_by_age;
};

void FormMens(CommandResults& result, vector<Person> people) {
    IteratorRange range{ people.begin(), partition(people.begin(), people.end(),
                                [/*g_m*/](const Person& p) {
                                    return p.is_male == true;
                                }) }; //
    if (range.begin() == range.end()) {
    }
    else {
        sort(range.begin(), range.end(),
            [](const Person& lhs, const Person& rhs) {
                return lhs.name < rhs.name;
            });
        const string* most_popular_name = &range.begin()->name;
        int count = 1;
        for (auto i = range.begin(); i != range.end();) {
            auto same_name_end = find_if_not(i, range.end(),
                [i](const Person& p) {
                    return p.name == i->name;
                });
            auto cur_name_count = distance(i, same_name_end);
            if (cur_name_count > count) {
                count = cur_name_count;
                most_popular_name = &i->name;
            }
            i = same_name_end;
        }
        result.most_popular_male_name = *most_popular_name;
    }
}
void FormWomans(CommandResults& result, vector<Person> people) {
    IteratorRange range_w{ people.begin(), partition(people.begin(), people.end(),
                                [](const Person& p) {
                                    return p.is_male == false;
                                }) }; //
    if (range_w.begin() == range_w.end()) {
    }
    else {
        sort(range_w.begin(), range_w.end(),
            [](const Person& lhs, const Person& rhs) {
                return lhs.name < rhs.name;
            });
        const string* most_popular_name = &range_w.begin()->name;
        int count = 1;
        for (auto i = range_w.begin(); i != range_w.end();) {
            auto same_name_end = find_if_not(i, range_w.end(),
                [i](const Person& p) {
                    return p.name == i->name;
                });
            auto cur_name_count = distance(i, same_name_end);
            if (cur_name_count > count) {
                count = cur_name_count;
                most_popular_name = &i->name;
            }
            i = same_name_end;
        }
        result.most_popular_female_name = *most_popular_name;
    }
}
void FormByAdge(CommandResults& result, vector<Person>& people) {
    sort(people.begin(), people.end(),
        [](const Person& lhs, const Person& rhs) {
            return lhs.age < rhs.age;
        });
    result.sorted_by_age = std::move(people);

}

void FormTotalIncome(CommandResults& result, vector<Person>& people) {
    const auto head_t = Head(people, people.size() - 1); //

    partial_sort(head_t.begin(), head_t.end(), people.end(),
        [](const Person& lhs, const Person& rhs) {
            return lhs.income > rhs.income;
        }); 
    for (size_t i = 0; i < people.size(); i++)
    {
        int total_income = accumulate(head_t.begin(), head_t.begin() + i, 0, [](int cur, const Person& p) {
            return p.income + cur;
            });
        result.cumulative_wealth.push_back(total_income);
    }
}

CommandResults BuildStatsData(vector<Person> people) {
    CommandResults result;


    FormTotalIncome(result, people);
    FormMens(result, people);
    FormWomans(result, people);
    FormByAdge(result, people);
    return result;
}

int main() {
    const CommandResults com = BuildStatsData(ReadPeople(cin));

    vector<Person> people = { people.begin(), people.end() };

    sort(people.begin(), people.end(),
        [](const Person& lhs, const Person& rhs) {
            return lhs.age < rhs.age;
        });

    for (string command; cin >> command;) {
        if (command == "AGE"s) {
            int adult_age;
            cin >> adult_age;

            auto adult_begin = lower_bound(com.sorted_by_age.begin(), com.sorted_by_age.end(), adult_age,
                [](const Person& lhs, int age) {
                    return lhs.age < age;
                });

            cout << "There are "s << distance(adult_begin, com.sorted_by_age.end()) << " adult people for maturity age "s
                << adult_age << '\n';
        }
        else if (command == "WEALTHY"s) {
            int count;
            cin >> count;

            cout << "Top-"s << count << " people have total income "s << com.cumulative_wealth[count] << '\n';

        }
        else if (command == "POPULAR_NAME"s) {
            char gender;
            cin >> gender;
            if (gender == 'M') {
                if (com.most_popular_male_name.has_value()) {
                    cout << "Most popular name among people of gender "s << gender << " is "s << com.most_popular_male_name.value() << '\n';
                }
                else {
                    cout << "No people of gender "s << gender << '\n';
                }
            }
            else if (gender == 'W') {
                if (com.most_popular_female_name.has_value()) {
                    cout << "Most popular name among people of gender "s << gender << " is "s << com.most_popular_female_name.value() << '\n';
                }
                else {
                    cout << "No people of gender "s << gender << '\n';
                }
            }
        }
    }
}