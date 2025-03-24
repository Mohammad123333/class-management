#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#define WINDOW "window"
#define DOOR "door"
#define MIDDLE "middle"
#define RESERVETABLE "reserve_table"
#define SWITCH "switch"
#define ENTER "enter"
#define SHOWTABLEINFO "show_table_info"
#define EXIT "exit"
using namespace std;

struct students;

typedef struct tables_info
{
    int ID;
    int X;
    int Y;
    int capacity;
    string type;
    vector<students *> students_in_table;
    vector<students *> student_in_queue;
    int type_bonus;
} tables_info;

typedef struct score
{
    tables_info *table;
    int score;

} score;

typedef struct students
{
    int ID;
    string name;
    int friend_ID;
    int enemy_ID;
    tables_info *main_table;
    vector<score> scores;
} students;

vector<students> read_students_from_csv(const string filename)
{
    vector<students> students_list;
    ifstream file(filename);

    string line;
    getline(file, line);
    while (getline(file, line))
    {
        stringstream ss(line);
        string id, name, friend_id, enemy_id;
        getline(ss, id, ',');
        getline(ss, name, ',');
        getline(ss, friend_id, ',');
        getline(ss, enemy_id, ',');

        students student;
        student.ID = stoi(id);
        student.name = name;
        student.friend_ID = stoi(friend_id);
        student.enemy_ID = stoi(enemy_id);
        student.main_table = nullptr;

        students_list.push_back(student);
    }

    file.close();
    return students_list;
}

vector<tables_info> read_tables_from_csv(const string filename)
{
    vector<tables_info> tables_list;
    ifstream file(filename);
    string line;
    getline(file, line);
    while (getline(file, line))
    {
        stringstream ss(line);
        string id, x, y, capacity, type;
        getline(ss, id, ',');
        getline(ss, x, ',');
        getline(ss, y, ',');
        getline(ss, capacity, ',');
        getline(ss, type, ',');

        tables_info table;
        table.ID = stoi(id);
        table.X = stoi(x);
        table.Y = stoi(y);
        table.capacity = stoi(capacity);
        table.type = type;
        if (type == WINDOW)
        {
            table.type_bonus = 6;
        }
        else if (type == DOOR)
        {
            table.type_bonus = 4;
        }
        else if (type == MIDDLE)
        {
            table.type_bonus = 2;
        }
        tables_list.push_back(table);
    }
    file.close();
    return tables_list;
}

int count_remaining_for_table(const tables_info &table)
{
    return table.capacity - table.students_in_table.size();
}

void sort_by_name_in_table(vector<tables_info> &tables)
{
    for (auto &table : tables)
    {
        sort(table.students_in_table.begin(), table.students_in_table.end(), [](students *a, students *b)
             { return a->name < b->name; });
    }
}

void show_table_info(vector<tables_info> &tables, int table_ID)
{
    sort_by_name_in_table(tables);

    for (const auto &table : tables)
    {
        if (table.ID == table_ID)
        {
            cout << "Table ID: " << table.ID << endl;
            cout << "People at the table: ";
            for (size_t i = 0; i < table.students_in_table.size(); i++)
            {
                cout << table.students_in_table[i]->name;
                if (i != table.students_in_table.size() - 1)
                {
                    cout << ", ";
                }
            }
            cout << endl;
            cout << "Table remaining capacity: " << count_remaining_for_table(table) << endl;
            cout << "Waiting queue length: " << table.student_in_queue.size() << endl;
            return;
        }
    }
}
vector<int> type_bonus_score(vector<tables_info> &table)
{
    vector<int> bonus_score(table.size(), 0);

    for (int i = 0; i < table.size(); i++)
    {
        if (table[i].type == WINDOW)
        {
            bonus_score[i] = 6;
        }
        else if (table[i].type == DOOR)
        {
            bonus_score[i] = 4;
        }
        else if (table[i].type == MIDDLE)
        {
            bonus_score[i] = 2;
        }
    }
    return bonus_score;
}

tables_info *find_current_table(vector<tables_info> &tables, int table_id)
{
    for (auto &table : tables)
    {
        if (table.ID == table_id)
        {
            return &table;
        }
    }
    return nullptr;
}
students *find_current(vector<students> &student_s, int stdid)
{
    for (auto &student : student_s)
    {
        if (student.ID == stdid)
            return &student;
    }
    return nullptr;
}

students *find_enemy(vector<students> &student_s, int stdid)
{
    students *current = find_current(student_s, stdid);
    for (auto &student_enemy : student_s)
    {
        if (current->enemy_ID == student_enemy.ID)
            return &student_enemy;
    }
    return nullptr;
}

students *find_friend(vector<students> &student_s, int stdid)
{
    students *current = find_current(student_s, stdid);
    for (auto &student_friend : student_s)
    {
        if (current->friend_ID == student_friend.ID)
            return &student_friend;
    }
    return nullptr;
}

vector<int> calculate_distance_to_enemy(vector<students> &student_s, int stdid, vector<tables_info> &tables)
{
    students *enemy = find_enemy(student_s, stdid);
    vector<int> enemy_distances(tables.size(), 0);
    if (enemy != nullptr && enemy->main_table != nullptr)
    {
        for (size_t i = 0; i < tables.size(); i++)
        {
            enemy_distances[i] = abs(enemy->main_table->X - tables[i].X) + abs(enemy->main_table->Y - tables[i].Y);
        }
    }
    return enemy_distances;
}

vector<int> calculate_distance_to_friend(vector<students> &student_s, int stdid, vector<tables_info> &tables)
{
    students *friend_stu = find_friend(student_s, stdid);
    vector<int> friend_distances(tables.size(), 0);
    if (friend_stu != nullptr && friend_stu->main_table != nullptr)
    {
        for (size_t i = 0; i < tables.size(); i++)
        {
            friend_distances[i] = abs(friend_stu->main_table->X - tables[i].X) + abs(friend_stu->main_table->Y - tables[i].Y);
        }
    }
    return friend_distances;
}

void calculate_each_table_score(vector<students> &student_s, int stdid, vector<tables_info> &tables)
{
    vector<int> each_table_score(tables.size(), 0);
    vector<int> distance_friend = calculate_distance_to_friend(student_s, stdid, tables);
    vector<int> distance_enemy = calculate_distance_to_enemy(student_s, stdid, tables);
    vector<int> type_bonus = type_bonus_score(tables);

    students *current_student = find_current(student_s, stdid);

    for (size_t i = 0; i < tables.size(); i++)
    {
        each_table_score[i] = -distance_friend[i] + distance_enemy[i] + type_bonus[i];
    }

    current_student->scores.clear();

    for (size_t i = 0; i < tables.size(); i++)
    {
        score s;
        s.table = &tables[i];
        s.score = each_table_score[i];
        current_student->scores.push_back(s);
    }
}

void sort_table_by_priority(students &student)
{
    sort(student.scores.begin(), student.scores.end(), [](const score &a, const score &b)
         { return a.score > b.score; });
}

void Enter(vector<students> &student_s, int stdid, vector<tables_info> &tables)
{
    students *current = find_current(student_s, stdid);

    calculate_each_table_score(student_s, stdid, tables);

    sort_table_by_priority(*current);

    for (const auto &table_score : current->scores)
    {
        const tables_info *table = table_score.table;

        int remaining_seats = count_remaining_for_table(*table);
        cout << "Table " << table->ID << ": " << remaining_seats << " " << table_score.table->student_in_queue.size() << endl;
    }
}

int check_table(tables_info &table)
{
    if (count_remaining_for_table(table) > 0)
    {
        return 1;
    }
    return 0;
}

void add_table_for_stu_after_reserve(tables_info *table, vector<students> &student_s, int stdid)
{
    students *current_student = find_current(student_s, stdid);
    if (check_table(*table))
    {
        current_student->main_table = table;
        table->students_in_table.push_back(current_student);
        cout << current_student->name << " sits at table " << table->ID << endl;
        return;
    }
    else
    {
        table->student_in_queue.push_back(current_student);
        cout << current_student->name << " enters the waiting queue of table " << table->ID << endl;
        return;
    }
}
void reserve_table(vector<tables_info> &tables, vector<students> &student_s, int table_id, int stdid)
{
    students *current_student = find_current(student_s, stdid);

    calculate_each_table_score(student_s, stdid, tables);
    sort_table_by_priority(*current_student);

    tables_info *table = find_current_table(tables, table_id);
    tables_info *best_table = current_student->scores[0].table;

    if (table_id == 0)
    {
        add_table_for_stu_after_reserve(best_table, student_s, stdid);
    }
    else
    {
        add_table_for_stu_after_reserve(table, student_s, stdid);
    }
}

void switch_with_freind(vector<students> &student_s, int stdid)
{
    students *current_student = find_current(student_s, stdid);
    students *freind = find_friend(student_s, stdid);

    tables_info *current_table = current_student->main_table;
    tables_info *freind_table = freind->main_table;

    auto it_current = find(current_table->students_in_table.begin(), current_table->students_in_table.end(), current_student);
    if (it_current != current_table->students_in_table.end())
    {
        current_table->students_in_table.erase(it_current);
    }

    auto it_freind = find(freind_table->students_in_table.begin(), freind_table->students_in_table.end(), freind);
    if (it_freind != freind_table->students_in_table.end())
    {
        freind_table->students_in_table.erase(it_freind);
    }

    current_student->main_table = freind_table;
    freind->main_table = current_table;
    freind_table->students_in_table.push_back(current_student);
    current_table->students_in_table.push_back(freind);
    cout << current_student->name << " switches seats with " << freind->name << "!" << endl;
}

int check_freind_in_queue(vector<students> &student_s, int stdid)
{
    students *current = find_current(student_s, stdid);
    students *freind = find_friend(student_s, stdid);

    for (int i = 0; i < current->main_table->student_in_queue.size(); i++)
    {
        if (freind->name == current->main_table->student_in_queue[i]->name)
        {
            return 1;
        }
    }
    return 0;
}

void exit_table(vector<students> &students_list, int stdid)
{
    students *current_student = find_current(students_list, stdid);

    students *freind = find_friend(students_list, stdid);
    tables_info *table = current_student->main_table;
    current_student->main_table = nullptr;

    auto it = find(table->students_in_table.begin(), table->students_in_table.end(), current_student);
    auto it_freind = find(table->student_in_queue.begin(), table->student_in_queue.end(), freind);

    if (it != table->students_in_table.end())
    {
        table->students_in_table.erase(it);
    }

    if (freind && it_freind != table->student_in_queue.end())
    {
        table->student_in_queue.erase(it_freind);
        freind->main_table = table;
        table->students_in_table.push_back(freind);
    }
    else if (!table->student_in_queue.empty())
    {
        students *next_student = table->student_in_queue.front();
        table->student_in_queue.erase(table->student_in_queue.begin());

        next_student->main_table = table;
        table->students_in_table.push_back(next_student);
    }

    cout << current_student->name << " exits!" << endl;
}
void add_tables_to_students(vector<students> &students, vector<tables_info> &tables)
{
    for (auto &student : students)
    {
        if (student.scores.size() < tables.size())
        {
            student.scores.resize(tables.size());
        }
        for (int i = 0; i < tables.size(); i++)
        {
            student.scores[i].table = &tables[i];
        }
    }
}

void handle_commands(string command, vector<students> students_list, vector<tables_info> tables_list)
{
    while (getline(cin, command))
    {
        size_t space_pos = command.find(' ');
        string cmd = command.substr(0, space_pos);
        string rest = (space_pos == string::npos) ? "" : command.substr(space_pos + 1);

        if (cmd == SHOWTABLEINFO)
        {
            stringstream ss(rest);
            int tableid;
            ss >> tableid;
            show_table_info(tables_list, tableid);
        }
        else if (cmd == RESERVETABLE || cmd == SWITCH || cmd == EXIT || cmd == ENTER)
        {
            stringstream ss(rest);
            int studentid;
            ss >> studentid;
            if (cmd == RESERVETABLE)
            {
                int tableid = 0;
                ss >> tableid;
                reserve_table(tables_list, students_list, tableid, studentid);
            }
            else if (cmd == SWITCH)
            {
                switch_with_freind(students_list, studentid);
            }
            else if (cmd == EXIT)
            {
                exit_table(students_list, studentid);
            }
            else if (cmd == ENTER)
            {
                Enter(students_list, studentid, tables_list);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    string tables_file = argv[1];
    string students_file = argv[2];
    vector<students> students_list = read_students_from_csv(students_file);
    vector<tables_info> tables_list = read_tables_from_csv(tables_file);
    string command;
    add_tables_to_students(students_list, tables_list);
    handle_commands(command, students_list, tables_list);
    return 0;
}
