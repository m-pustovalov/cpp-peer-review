#include <iostream>
#include <iomanip>
#include <vector>

class EBook {
public:
    using user_id_t = uint32_t;
    using page_number_t = uint16_t;

    EBook()
    : users_num_(0)
    , users_progress_{0}
    , num_users_cache_{0} {
    }

    void Read(user_id_t user_id, page_number_t page_number) {
        page_number_t prev_page_number = users_progress_[user_id];
        if (prev_page_number == 0) {
            ++users_num_;
        }
        users_progress_[user_id] = page_number;
        for (page_number_t i = prev_page_number + 1; i <= page_number; ++i) {
            ++num_users_cache_[i];
        }
    }

    double GetFractionOfUsersWhoReadLess(user_id_t user_id) {
        page_number_t pages_read = users_progress_[user_id];
        if (pages_read == 0) {
            return 0;
        }
        if (users_num_ == 1) {
            return 1;
        }
        return (users_num_ - num_users_cache_[pages_read]) / (users_num_ - 1.0);
    }

private:
    user_id_t users_num_;
    page_number_t users_progress_[100000 + 1];
    user_id_t num_users_cache_[1000 + 1];
};


void ReadAndExecuteCommands(std::istream& in, std::ostream& out) {
    uint32_t num_queries;
    in >> num_queries;

    EBook book;

    std::string command;
    EBook::user_id_t user_id;
    EBook::page_number_t page_number;

    out << std::setprecision(6);
    while (num_queries--) {
        in >> command >> user_id;
        if (command == "READ") {
            in >> page_number;
            book.Read(user_id, page_number);
        } else { // "CHEER"
            out << book.GetFractionOfUsersWhoReadLess(user_id) << std::endl;
        }
    }
}


int main() {
    ReadAndExecuteCommands(std::cin, std::cout);
    return 0;
}
