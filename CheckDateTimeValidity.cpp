struct DateTime {
    enum ValidationResult {
        OK,
        INVALID_YEAR,
        INVALID_MONTH,
        INVALID_DAY,
        INVALID_HOUR,
        INVALID_MINUTE,
        INVALID_SECOND
    };

    int year;
    int month;
    int day;

    int hour;
    int minute;
    int second;

     ValidationResult GetValidationResult() const {
        if (year < 1 || year > 9999) {
            return INVALID_YEAR;
        }

        if (month < 1 || month > 12) {
            return INVALID_MONTH;
        }

        const bool is_leap_year = (year % 4 == 0) && !(year % 100 == 0 && year % 400 != 0);
        const array month_lengths = {31, 28 + is_leap_year, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (day < 1 || day > month_lengths[month-1]) {
            return INVALID_DAY;
        }

        if (hour < 0 || hour > 23) {
            return INVALID_HOUR;
        }
        if (minute < 0 || minute > 59) {
            return INVALID_MINUTE;
        }
        if (second < 0 || second > 59) {
            return INVALID_SECOND;
        }
    }

};

void CheckDateTimeValidity(const DateTime& dt) {
    DateTime::ValidationResult valid_res = dt.GetValidationResult();
    if (valid_res == DateTime::OK) {
        return;
    }

    switch (valid_res) {
        case DateTime::INVALID_YEAR:
            throw domain_error("year is out of range"s);
        case DateTime::INVALID_MONTH:
            throw domain_error("month is out of range"s);
        case DateTime::INVALID_DAY:
            throw domain_error("day is out of range"s);

        case DateTime::INVALID_HOUR:
            throw domain_error("hour is out of range"s);
        case DateTime::INVALID_MINUTE:
            throw domain_error("minute is out of range"s);
        case DateTime::INVALID_SECOND:
            throw domain_error("second is out of range"s);
    }
}