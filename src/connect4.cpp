#include "connect4.hpp"
#include <ostream>

void State::dump(std::ostream& s) const
{
    for (int i = 5; i >= 0; i--) {
        for (int j = 0; j < 7; ++j)
            s << symbol(get(i,j)) << "  ";
        s << std::endl;
    }
    s << "0  1  2  3  4  5  6" << std::endl;
//        for (int i = 0; i < 7; ++i)
//            s << column_height(i) << " ";
    s << " last = " << last_column() << " winner = " << winner() << std::endl;
}
