#include "rope.h"

namespace rope {

t::t(std::string&& s) 
: _tag(tag::STRING) {

    new (&_string) std::string(std::move(s));
}

t::t(std::unique_ptr<t> lhs, 
     std::unique_ptr<t> rhs) 
: _tag(tag::APPEND) {

    new (&_append) t::append(std::move(lhs), 
                             std::move(rhs));
}

t::~t() {
    using std::string; 
    switch(_tag) {
    case tag::STRING: _string.~string(); break;
    case tag::APPEND: _append.~append(); break;
    }
}

t::append::append(std::unique_ptr<t> lhs, 
                  std::unique_ptr<t> rhs)
: _lhs(std::move(lhs))
 ,_rhs(std::move(rhs)) { 

}


} // namespace rope

int main () {
    rope::t s(std::string("s"));

    return 0;
}
