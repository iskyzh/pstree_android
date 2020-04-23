#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <cassert>

using namespace std;

bool starts_with(const string& a, const string& b) {
    return a.find(b) == 0;
}

int get_int(stringstream &ss) {
    int x = -1;
    while (ss) {
        if (ss.get() == '[') {
            ss >> x;
            return x;
        }
    }
    cerr << "get int failed" << endl;
    assert(x != -1);
    return -1;
}

string role(const string& entry) {
    int space = entry.find(' ');
    return entry.substr(0, space);
}

int id_of(const string& entry) {
    stringstream ss(entry);
    return get_int(ss);
}

string action(const string& entry) {
    stringstream ss(entry);
    get_int(ss);
    string x;
    ss >> x;
    ss >> x;
    if (x[x.length() - 1] == '.') x.pop_back();
    return x;
}


int cook, customer, cashier, rack;

void test_case(const string& case_name) {
    cout << "  " << case_name << " ... ";
}

bool test_assert(bool x) {
    if (x) cout << "\033[32mOK";
    else cout << "\033[31mFailed";
    cout << "\033[39m" << endl;
    return x;
}

bool should_never_exceed_rack_size(const vector<string> &entries) {
    test_case("cook present correct burgers");
    int on_rack = 0;
    for (auto &&x : entries) {
        auto act = action(x);
        if (act == "make") on_rack += 1;
        if (act == "take") on_rack -= 1;
        if (on_rack < 0) return false;
        if (on_rack > rack) return false;
    }
    return true;
}

bool should_have_correct_customers(const vector<string> &entries) {
    test_case("customers come and get burger");
    int on_rack = 0;
    vector <bool> customers;
    customers.resize(customer);
    for (auto &&x : entries) {
        auto act = action(x);
        auto r = role(x);
        auto id = id_of(x);
        if (r == "Customer") {
            if (act == "come") {
                if (customers[id]) return false;
                customers[id] = true;
            }
            if (act == "get") {
                if (!customers[id]) return false;
                customers[id] = false;
            }
        }
    }
    return true;
}

bool should_serve_one_customer(const vector<string> &entries) {
    test_case("cashier only serve one customer at a time");
    int on_rack = 0;
    vector <bool> cashiers;
    cashiers.resize(cashier);
    for (auto &&x : entries) {
        auto act = action(x);
        auto r = role(x);
        auto id = id_of(x);
        if (r == "Cashier") {
            if (act == "accepts") {
                if (cashiers[id]) return false;
                cashiers[id] = true;
            }
            if (act == "take") {
                if (!cashiers[id]) return false;
                cashiers[id] = false;
            }
        }
    }
    return true;
}


bool should_have_log(const vector<string> &entries) {
    test_case("should be at least 50 log entries");
    return entries.size() >= 50;
}

int main(int argc, char* argv[]) {
    string file_path;
    vector <string> log_entry;
    if (argc == 2) {
        file_path = argv[1];
    } else {
        cerr << "usage: test logfile\n";
        return 1;
    }
    clog << "Parsing log file " << file_path << endl;
    ifstream fin(file_path);
    if (!fin) {
        cerr << "Failed to open log file " << file_path << endl;
        return 1;
    }

    while (fin) {
        string entry;
        getline(fin, entry);
        if (entry.find('.') != string::npos) {
            log_entry.push_back(entry);
        }
        if (starts_with(entry, "Cooks")) {
            stringstream ss(entry);
            cook = get_int(ss);
            cashier = get_int(ss);
            customer = get_int(ss);
            rack = get_int(ss);
            clog << "Task info: Cook=" << cook << " Cashier=" << cashier << " Customer=" << customer << " Rack=" << rack << endl;
        }

        if (starts_with(entry, "Begin run.")) {
            log_entry.clear();
            clog << "Found start point." << endl;
        }
    }
    clog << log_entry.size() << " entries read." << endl;
    string first_entry = *log_entry.begin();
    clog << "first entry: role=" << role(first_entry) << " id=" << id_of(first_entry) << " action=" << action(first_entry) << endl;
#ifdef TRACE
    for (auto entry: log_entry) {
        clog << "role=" << role(entry) << " id=" << id_of(entry) << " action=" << action(entry) << endl;
    }
#endif
    bool test_passed = true;
    test_passed &= test_assert(should_have_log(log_entry));
    test_passed &= test_assert(should_never_exceed_rack_size(log_entry));
    test_passed &= test_assert(should_have_correct_customers(log_entry));
    test_passed &= test_assert(should_serve_one_customer(log_entry));
    return !test_passed;
}
