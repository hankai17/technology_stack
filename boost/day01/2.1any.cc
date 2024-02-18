#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>
#include <algorithm>
#include <boost/any.hpp>

typedef boost::any cell_t;
typedef std::vector<cell_t> db_row_t;

db_row_t get_row(const char* query) {
    db_row_t row;
    row.push_back(10);
    row.push_back(10.1f);
    row.push_back(std::string("melon"));
    return row;
}

struct db_sum: 
    public std::unary_function<boost::any, void> {
public:
    explicit db_sum(double& sum)
        : sum_(sum) {}

    void operator() (const cell_t& value) {
        const std::type_info& ti = value.type(); // any对外暴露type函数
        if (ti == typeid(int)) {
            sum_ += boost::any_cast<int>(value);
        } else if (ti == typeid(float)) {
            sum_ += boost::any_cast<float>(value);
        }
        // 不做处理
    }
private:
    double& sum_;
};

int main()
{
    double res = 0.0;
    db_row_t row = get_row("Query: Give me some row, please."); // vecs<any>

    std::for_each(row.begin(), row.end(), db_sum(res));
    std::cout << "Sum of arithmetic types in database row is: " << res << std::endl;
    return 0;
}
