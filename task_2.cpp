#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>
#include <unordered_map>

using namespace std;

class Domain {
public:
    friend class DomainChecker;

    Domain(const string& str) {
        istringstream stream(str);
        string word;
        while (getline(stream, word, '.')) {
            reversed_parts_.push_back(word);
        }
        std::reverse(reversed_parts_.begin(), reversed_parts_.end());
    }

    bool operator==(const Domain& other) {
        return reversed_parts_ == other.reversed_parts_;
    }

    // Возвращает true, если this его поддомен
    // NB: not used in my solution
    bool IsSubdomain(const Domain& other) {
        if (reversed_parts_.size() > other.reversed_parts_.size()) {
            return false;
        }
        for (size_t i = 0; i < reversed_parts_.size(); ++i) {
            if (reversed_parts_[i] != other.reversed_parts_[i]) {
                return false;
            }
        }
        return true;
    }

private:
    vector<string> reversed_parts_;
};


class DomainChecker {
public:
    template <typename It>
    DomainChecker(It start, It end) {
        for (auto it = start; it != end; ++it) {
            Domain d{*it};
            DomainMap* cur_level = &dmap_;
            for (const string& w : d.reversed_parts_) {
                if (cur_level->data.count(w) == 0) {
                    cur_level->data[w] = new DomainMap();
                }
                cur_level = cur_level->data.at(w);
            }
            cur_level->is_leaf = true;
        }
    }

    bool IsForbidden(const Domain& domain) {
        DomainMap* cur_level = &dmap_;
        for (const string& w : domain.reversed_parts_) {
            if (cur_level->data.count(w) == 0) {
                return cur_level->is_leaf;
            }
            cur_level = cur_level->data.at(w);
            if (cur_level->data.size() == 0 || cur_level->is_leaf) {
                return true;
            }
        }
        return cur_level->is_leaf;
    }

    ~DomainChecker() {
        CleanUp(&dmap_);
    }

private:
    struct DomainMap {
        unordered_map<string, DomainMap*> data;
        bool is_leaf = false;
    };

    void CleanUp(DomainMap* dm) {
        for (auto [_key, value] : dm->data) {
            CleanUp(value);
            delete value;
        }
    }

    DomainMap dmap_;
};


vector<Domain> ReadDomains(istream& input, size_t num_domains) {
    vector<Domain> result;
    result.reserve(num_domains);
    string line;
    while (num_domains > 0 && getline(input, line)) {
        result.push_back({line});
        --num_domains;
    }
    return result;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}


// Tests
void TestSubdomains() {
    assert(Domain("").IsSubdomain({""}));
    assert(Domain("").IsSubdomain({"com"}));
    assert(Domain("com").IsSubdomain({"test.com"}));
    assert(Domain("second.first").IsSubdomain({"third.second.first"}));
}

void TestEqOp() {
    assert(Domain("") == Domain(""));
    assert(Domain("com") == Domain("com"));
    assert(Domain("domain.com") == Domain("domain.com"));
    assert(Domain("1.2.3.4.5.6") == Domain("1.2.3.4.5.6"));
}

void TestChecker() {
    vector<Domain> forbidden{{"com"},
                             {"a.ru"},
                             {"c.b.ru"}};
    DomainChecker checker{forbidden.begin(), forbidden.end()};
    assert(checker.IsForbidden({"company.com"}));
    assert(checker.IsForbidden({"a.ru"}));
    assert(checker.IsForbidden({"sub.a.ru"}));
    assert(checker.IsForbidden({"c.b.ru"}));
    assert(checker.IsForbidden({"sub.c.b.ru"}));

    assert(!checker.IsForbidden({"b.ru"}));
    assert(!checker.IsForbidden({"a.b.ru"}));
    assert(!checker.IsForbidden({"d.ru"}));
    assert(!checker.IsForbidden({"a.b.c"}));
}

void RunTests() {
    TestSubdomains();
    TestEqOp();
    TestChecker();
}


int main() {
    RunTests();

    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}