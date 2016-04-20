#include "rope.h"

namespace rope {

t::t(std::string&& s) 
: _tag(tag::STRING), 
  _height(1), 
  _size(s.size()) {

    new (&_string) std::string(std::move(s));
}

t::t(t&& lhs, 
     t&& rhs) 
: _tag(tag::APPEND), 
  _height(std::max(lhs._height, rhs._height) + 1), 
  _size(lhs._size + rhs._size) {

    new (&_append) t::append(std::move(lhs), 
                             std::move(rhs));
}
  
t::t(t&& copy) {
    
  using std::string; 

  switch(copy._tag) {
  case tag::STRING: new (&_string) string(std::move(copy._string));break;
  case tag::APPEND: new (&_append) append(std::move(copy._append));break;
  }
  
  _tag    = copy._tag;
  _size   = copy._size;
  _height = copy._height;
}

t::~t() {
  using std::string; 
  switch(_tag) {
  case tag::STRING: _string.~string(); break;
  case tag::APPEND: _append.~append(); break;
  }
}

std::size_t t::size() const {
  return _size;
}

std::size_t t::height() const {
  return _height;
}

char& t::operator[](std::size_t index) {
  switch(_tag) {

  case tag::STRING: {
    return _string[index];
  } break;

  case tag::APPEND: {
    std::size_t lhs_size = _append._lhs->_size; 
    if(index >= lhs_size) {
      return _append._rhs->operator[](index - lhs_size);
    }
    else {
      return _append._lhs->operator[](index);
    }
  } break;

  } // switch
}

char t::operator[](std::size_t index) const {
    t& self = const_cast<t&>(*this); 
    return self[index];
}

t& t::append_string(std::string&& s) {

    std::size_t s_size = s.size();

    switch(_tag) {

    case tag::STRING:{
      
      append a(std::move(*this), 
               t(std::move(s)));
      
      using std::string;
      _string.~string();
      new (&_append) append(std::move(a)); 
      _tag    = tag::APPEND;

    } break;
    
    case tag::APPEND:{
      
      if(_append._rhs->_height >= _append._lhs->_height) {
        // rebalance on the other side.
        
        append a(std::move(*this), 
                 t(std::move(s)));
      
        _append.~append();
        new (&_append) append(std::move(a)); 
        _tag    = tag::APPEND;
      }
      else {
        // append on the rhs which has room.
        _append._rhs->append_string(std::move(s));
      }

    } break;

    }

    _size  += s_size; 
    _height = std::max(_append._rhs->_height, _append._lhs->_height) + 1; 

    return *this;
}
  
iterator t::begin() {
  return iterator(this, 0);
}

iterator t::end() {
  return iterator(this, _size);
}

const_iterator t::begin() const {
  return const_iterator(this, 0);
}

const_iterator t::end() const {
  return const_iterator(this, _size);
}

t::append::append(t&& lhs, 
                  t&& rhs)
: _lhs(new t(std::move(lhs)))
 ,_rhs(new t(std::move(rhs))) { 

}

// Iterators
// ---------

iterator::iterator() 
: _ptr(nullptr), 
  _index(0)
{}

iterator::iterator(t* rope_ptr, 
                      std::size_t index)
: _ptr(rope_ptr), 
  _index(index)
{ }

iterator::iterator(iterator const& copy)
: _ptr(copy._ptr), 
  _index(copy._index)
{
}

iterator::reference iterator::operator*() {
  return _ptr->operator[](_index);
}

iterator& iterator::operator++() {
  ++_index;
  return *this;
}

iterator iterator::operator++(int) {
    iterator tmp(*this); 
    ++_index;
    return tmp;
}

bool iterator::operator==(iterator const& rhs) const {
    return _ptr   == rhs._ptr && 
           _index == rhs._index;
}

bool iterator::operator!=(iterator const& rhs) const {
    return !this->operator==(rhs);
}

const_iterator::const_iterator() 
: _ptr(nullptr), 
  _index(0)
{}

const_iterator::const_iterator(t const * rope_ptr, 
                               std::size_t index)
: _ptr(rope_ptr), 
  _index(index)
{ }

const_iterator::const_iterator(const_iterator const& copy)
: _ptr(copy._ptr), 
  _index(copy._index)
{
}

const_iterator::value_type const_iterator::operator*() {
  return _ptr->operator[](_index);
}

const_iterator& const_iterator::operator++() {
  ++_index;
  return *this;
}

const_iterator const_iterator::operator++(int) {
    const_iterator tmp(*this); 
    ++_index;
    return tmp;
}

bool const_iterator::operator==(const_iterator const& rhs) const {
    return _ptr   == rhs._ptr && 
           _index == rhs._index;
}

bool const_iterator::operator!=(const_iterator const& rhs) const {
    return !this->operator==(rhs);
}


} // namespace rope
