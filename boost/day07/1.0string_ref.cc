#include <string>
#include <algorithm>

std::string between_str(const std::string& input, char starts, char ends) {
    std::string::const_iterator pos_beg = std::find(input.begin(), input.end(), starts);
    if (pos_beg == input.end()) {
        return std::string();
    }

    ++pos_beg;
    std::string::const_iterator pos_end = std::find(pos_beg, input.end(), ends);
    return std::string(pos_beg, pos_end);
}

#include <boost/utility/string_ref.hpp> // c17中的string_view 其内部只存放ptr_跟len_
boost::string_ref between(const boost::string_ref& input, char starts, char ends) 
{
    boost::string_ref::const_iterator pos_beg = std::find(input.cbegin(), input.cend(), starts);
    if (pos_beg == input.cend()) {
        return boost::string_ref();
    }

    ++pos_beg;
    boost::string_ref::const_iterator pos_end = std::find(pos_beg, input.cend(), ends);
    if (pos_end == input.cend()) {
        return boost::string_ref(pos_beg, input.end() - pos_beg);
    }
    return boost::string_ref(pos_beg, pos_end - pos_beg);
}

#include <vector>
void string_ref_init_examples() {
    boost::string_ref r0("^_^");

    std::string O_O("O__O");
    boost::string_ref r1 = O_O;

    std::vector<char> chars_vec(10, '#');
    boost::string_ref r2(&chars_vec.front(), chars_vec.size());

    (void)r0;
    (void)r1;
    (void)r2;
}

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <iterator>
#include <iostream>
void string_ref_algorithms_examples() {
    boost::string_ref r("O_O");
    std::find(r.cbegin(), r.cend(), '_');

    boost::to_lower_copy(std::ostream_iterator<char>(std::cout), r);
    std::cout << '\n';

    std::cout << r << '\n';

    boost::replace_all_copy(std::ostream_iterator<char>(std::cout), r, "O", "^");
    std::cout << '\n';

    r = "100";
    assert(boost::lexical_cast<int>(r) == 100);
}

int main() {
    std::cout << between("Getting expression (between brackets)", '(', ')') << '\n';
    std::cout << between(std::string("(expression)"), '(', ')') << '\n';

    string_ref_init_examples();
    string_ref_algorithms_examples();
    return 0;
}

