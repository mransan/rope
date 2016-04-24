#include "rope.h"
#include <cassert>

namespace rope {

t::t(std::string&& s) 
: _tag(tag::STRING), 
  _height(1), 
  _size(s.size()),
  _parent(nullptr) { 

  new (&_string) std::string(std::move(s));
}

t::t(t&& lhs, 
     t&& rhs) 
: _tag(tag::APPEND), 
  _height(std::max(lhs._height, rhs._height) + 1), 
  _size(lhs._size + rhs._size), 
  _parent(nullptr) {

  new (&_append) t::append(std::move(lhs), 
                           std::move(rhs), 
                           this);
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
  _parent = copy._parent;
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
               t(std::move(s)), 
               this);
      
      using std::string;
      _string.~string();
      new (&_append) append(std::move(a), this); 
      _tag    = tag::APPEND;

    } break;
    
    case tag::APPEND:{
      
      if(_append._rhs->_height >= _append._lhs->_height) {
        // rebalance on the other side.
        
        append a(std::move(*this), 
                 t(std::move(s)), 
                 this);
      
        _append.~append();
        new (&_append) append(std::move(a), this); 
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

t* t::left_most_string() {
  if(_tag == tag::STRING) {
    return this;
  }
  else {
    return _append._lhs->left_most_string();
  }
}
  
iterator t::begin() {
  return iterator(this->left_most_string(), 0);
}

iterator t::end() {
  return iterator();
}

const_iterator t::begin() const {
  t* self = const_cast<t*>(this);
  return const_iterator(self->left_most_string(), 0);
}

const_iterator t::end() const {
  return const_iterator();
}

t::append::append(t&& lhs, 
                  t&& rhs, 
                  t*   self)
: _lhs(new t(std::move(lhs)))
 ,_rhs(new t(std::move(rhs))) { 

  _lhs->_parent = self;
  _rhs->_parent = self;
}

t::append::append(append&& copy, t* self)
: _lhs(std::move(copy._lhs)), 
  _rhs(std::move(copy._rhs)) {

  _lhs->_parent = self;
  _rhs->_parent = self;
}

t * t::next_string_leaf() {
  if(_parent == nullptr) {
    // root node
    return nullptr;
  }

  assert(_parent->_tag == tag::APPEND); 
    // Check invariant.

  bool am_i_lhs = _parent->_append._lhs.get() == this; 
  if(am_i_lhs) {
    // I am the lhs side node of my parent, 
    // the next one is my sibling the rhs. 
    return _parent->_append._rhs.get (); 
  }

  // I am the rhs side of my parent, we need to 
  // go up a level and do this again to reach cousins/uncles/

  return _parent->next_string_leaf();
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
{ 
  assert(_ptr != nullptr); 
  assert(_ptr->_tag == t::tag::STRING);
}

iterator::iterator(iterator const& copy)
: _ptr(copy._ptr), 
  _index(copy._index)
{
}

iterator::reference iterator::operator*() {
  assert(_ptr != nullptr); 
  assert(_ptr->_tag == t::tag::STRING);
  return _ptr->_string[_index];
}

iterator& iterator::operator++() {
  assert(_ptr->_tag == t::tag::STRING);
  ++_index;
  if(_index == _ptr->_string.size()) {
      _ptr   = _ptr->next_string_leaf();
      _index = 0; 
  };
  return *this;
}

iterator iterator::operator++(int) {
  iterator tmp(*this); 
  assert(_ptr->_tag == t::tag::STRING);
  ++_index;
  if(_index == _ptr->_string.size()) {
      _ptr   = _ptr->next_string_leaf();
      _index = 0; 
  };
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
: _i() 
{}

const_iterator::const_iterator(t const * rope_ptr, 
                               std::size_t index)
: _i(const_cast<t*>(rope_ptr), index)
{ }

const_iterator::const_iterator(const_iterator const& copy)
: _i(copy._i)
{
}

const_iterator::value_type const_iterator::operator*() {
  return *_i;
}

const_iterator& const_iterator::operator++() {
  ++_i;
  return *this;
}

const_iterator const_iterator::operator++(int) {
    const_iterator tmp(*this); 
    ++_i;
    return tmp;
}

bool const_iterator::operator==(const_iterator const& rhs) const {
    return _i == rhs._i;
}

bool const_iterator::operator!=(const_iterator const& rhs) const {
    return _i != rhs._i;
}


} // namespace rope
