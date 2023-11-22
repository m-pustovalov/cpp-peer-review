struct DBConnectionSettings {
    string_view name;
    int timeout;
    bool allow_exceptions;
    DBLogLevel log_level;
};

class QueryFilter {
public:
    QueryFilter(string column_name)
    : column_name_(column_name) {
    }

    string GetQueryString() {
        ostringstream os;
        os << column_name_ << ' ';
        PrintQuery(os);
        return os.str();
    }

protected:
    virtual void PrintQuery(ostringstream& os) = 0;

    string Quote(string src) {
        return src;  // safely quote input str
    }

private:
    string column_name_;
};


class RangeFilter : public QueryFilter {
public:
    RangeFilter(string column_name, int min_value, int max_value)
    : QueryFilter(column_name)
    , min_value_(min_value)
    , max_value_(max_value) {
    }

protected:
    void PringQuery(ostringstream& os) override {
        os << "between " << min_value_ << " and " << max_value_;
    }

private:
    int min_value_;
    int max_value_;
};

class LikeFilter : public QueryFilter {
public:
    LikeFilter(string column_name, string pattern)
        : QueryFilter(column_name)
        , pattern_(pattern) {
    }

protected:
    void PringQuery(ostringstream& os) override {
        os << "like '%" << Quote(pattern_) << "%'";
    }

private:
    string pattern_;
};

// ToDo: user builder pattern to catch common mistakes in compile time
class DBQuery {
public:
    DBQuery() {
    }

    DBQuery SetTable(string table_name) {
        table_name_ = table_name;
        return *this;
    }

    DBQuery AddColumn(string column) {
        columns_.push_back(column);
        return *this;
    }

    DBQuery AddFilter(QueryFilter* filter) {
        filters_.push_back(filter);
        return *this;
    }

    string Build() {
        ostringstream os;
        os << "from " << table_name_ << " select ";
        PrintColumns(os);
        os << " where ";
        PringFilters(os);
        return os.str();
    }


private:

    void PrintColumns(ostringstream& os) {
        if (columns_.size() == 0) {
            return;
        }
        os << columns_[0];
        for (size_t i = 1; i < columns_.size(); ++i) {
            os << ", " << columns_[i];
        }
    }

    void PringFilters(ostringstream& os) {
        if (filters_.size() == 0) {
            return;
        }
        os << filters_[0]->GetQueryString();
        for (size_t i = 1; i < filters_.size(); ++i) {
            os << " and " << filters_[i]->GetQueryString();
        }
    }

    string table_name_;
    vector<string> columns_;
    vector<QueryFilter*> filters_;
};

class QueryBuilder {
public:
    QueryBuilder() {
    }

    QueryBuilder SetTable(string_view table_name) {

    }
private:
    DBQuery query;
};


vector<Person> LoadPersons(string_view db_name, int db_connection_timeout, bool db_allow_exceptions,
                           DBLogLevel db_log_level, int min_age, int max_age, string_view name_filter) {
    DBConnector connector(conn_settings.allow_exceptions, conn_settings.log_level);
    DBHandler db;
    if (conn_settings.name.starts_with("tmp."s)) {
        db = connector.ConnectTmp(conn_settings.name, conn_settings.timeout);
    } else {
        db = connector.Connect(conn_settings.name, conn_settings.timeout);
    }
    if (!conn_settings.allow_exceptions && !db.IsOK()) {
        return {};
    }

    DBQuery q;
    q.SetTable("Persons")
     .AddColumn("Name")
     .AddColumn("Age")
     .AddFilter(new RangeFilter("Age", min_age, max_age))
     .AddFilter(new LikeFilter("Name", name_filter));

    DBQuery query(q.Build());

    vector<Person> persons;
    for (auto [name, age] : db.LoadRows<string, int>(query)) {
        persons.push_back({move(name), age});
    }
    return persons;
}