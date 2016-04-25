#include "rope.h"
#include <cassert>

#ifdef MRANSAN_ROPE_DEBUG
#define ASSERT(x) assert(x)
#else
#define ASSERT(x) 
#endif

namespace rope {

t::t(std::string const& s) 
: tag_(tag::STRING), 
  height_(1), 
  size_(s.size()),
  parent_(nullptr) { 

  new (&_string) std::string(s);
}

t::t(std::string&& s) 
: tag_(tag::STRING), 
  height_(1), 
  size_(s.size()),
  parent_(nullptr) { 

  new (&_string) std::string(std::move(s));
}

t::t(t&& lhs, 
     t&& rhs) 
: tag_(tag::APPEND), 
  height_(std::max(lhs.height_, rhs.height_) + 1), 
  size_(lhs.size_ + rhs.size_), 
  parent_(nullptr) {

  new (&_append) t::append(std::move(lhs), 
                           std::move(rhs), 
                           this);
}
  
t::t(t&& copy) {
    
  using std::string; 

  switch(copy.tag_) {
  case tag::STRING: new (&_string) string(std::move(copy._string));break;
  case tag::APPEND: new (&_append) append(std::move(copy._append));break;
  }
  
  tag_    = copy.tag_;
  size_   = copy.size_;
  height_ = copy.height_;
  parent_ = copy.parent_;
}

t::~t() {
  using std::string; 
  switch(tag_) {
  case tag::STRING: _string.~string(); break;
  case tag::APPEND: _append.~append(); break;
  }
}

std::size_t t::size() const {
  return size_;
}

std::size_t t::height() const {
  return height_;
}

char& t::operator[](std::size_t index) {
  switch(tag_) {

  case tag::STRING: {
    return _string[index];
  } break;

  case tag::APPEND: {
    std::size_t lhssize_ = _append._lhs->size_; 
    if(index >= lhssize_) {
      return _append._rhs->operator[](index - lhssize_);
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

  std::size_t ssize_ = s.size();

  switch(tag_) {

  case tag::STRING:{
    
    append a(std::move(*this), 
             t(std::move(s)), 
             this);
    
    using std::string;
    _string.~string();
    new (&_append) append(std::move(a), this); 
    tag_    = tag::APPEND;

  } break;
  
  case tag::APPEND:{
    
    if(_append._rhs->height_ >= _append._lhs->height_) {
      // rebalance on the other side.
      
      append a(std::move(*this), 
               t(std::move(s)), 
               this);
    
      _append.~append();
      new (&_append) append(std::move(a), this); 
      tag_    = tag::APPEND;
    }
    else {
      // append on the rhs which has room.
      _append._rhs->append_string(std::move(s));
    }

  } break;

  }

  size_  += ssize_; 
  height_ = std::max(_append._rhs->height_, _append._lhs->height_) + 1; 

  return *this;
}

t& t::append_string(std::string const& rhs) {
  return this->append_string(std::string(rhs));
}

t* t::left_most_string() {
  if(tag_ == tag::STRING) {
    return this;
  }
  else {
    return _append._lhs->left_most_string();
  }
}
  
iterator t::begin() {
  if(size_ == 0) {
    return iterator();
  }
  else {
    return iterator(this->left_most_string(), 0);
  }
}

iterator t::end() {
  return iterator();
}

const_iterator t::begin() const {
  if(size_ == 0) {
    return const_iterator();
  }
  else {
    t* self = const_cast<t*>(this);
    return const_iterator(self->left_most_string(), 0);
  }
}

const_iterator t::end() const {
  return const_iterator();
}

t::append::append(t&& lhs, 
                  t&& rhs, 
                  t*   self)
: _lhs(new t(std::move(lhs)))
 ,_rhs(new t(std::move(rhs))) { 

  _lhs->parent_ = self;
  _rhs->parent_ = self;
}

t::append::append(append&& copy, t* self)
: _lhs(std::move(copy._lhs)), 
  _rhs(std::move(copy._rhs)) {

  _lhs->parent_ = self;
  _rhs->parent_ = self;
}

t* t::next_string_leaf() {
  if(parent_ == nullptr) {
    // root node
    return nullptr;
  }

  ASSERT(parent_->tag_ == tag::APPEND); 
    // Check invariant.

  bool am_i_lhs = parent_->_append._lhs.get() == this; 
  if(am_i_lhs) {
    // I am the lhs side node of my parent, 
    // the next one is my sibling the rhs. 
    return parent_->_append._rhs.get (); 
  }

  // I am the rhs side of my parent, we need to 
  // go up a level and do this again to reach cousins/uncles/

  return parent_->next_string_leaf();
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
  ASSERT(_ptr != nullptr); 
  ASSERT(_ptr->tag_ == t::tag::STRING);
}

iterator::iterator(iterator const& copy)
: _ptr(copy._ptr), 
  _index(copy._index)
{
}

iterator::reference iterator::operator*() {
  ASSERT(_ptr != nullptr); 
  ASSERT(_ptr->tag_ == t::tag::STRING);
  return _ptr->_string[_index];
}

iterator& iterator::operator++() {
  ASSERT(_ptr->tag_ == t::tag::STRING);
  ++_index;
  if(_index == _ptr->_string.size()) {
      _ptr   = _ptr->next_string_leaf();
      _index = 0; 
  };
  return *this;
}

iterator iterator::operator++(int) {
  iterator tmp(*this); 
  ASSERT(_ptr->tag_ == t::tag::STRING);
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
{ }

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
