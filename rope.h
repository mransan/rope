#ifndef __MRANSAN__ROPE__H__
#define __MRANSAN__ROPE__H__

#include <string>
#include <memory>
#include <iterator>

namespace rope {

class t; 
class iterator;
class const_iterator;


// @brief Rope Class which implements basic functionality 
// of the Rope data structure for std::string. 
//
// https://en.wikipedia.org/wiki/Rope_(data_structure) 
class t {

public:
  // Creators
  // --------

  explicit t(std::string&& s);

  t(t&& lhs, 
    t&& rhs);

  t(t&& copy);

  // Destructors 
  // -----------

  ~t();

  // Accessors
  // ---------

  std::size_t size() const;

  std::size_t height() const;
    // Internal height of the rope tree data structure
    // Only used for testing

  char& operator[](std::size_t index);
  char  operator[](std::size_t index) const;

  // Modifiers
  // ---------
  
  t& append_string(std::string&& rhs); 

  // Iterators
  // ---------
  
  iterator begin(); 
  iterator end(); 

  const_iterator begin() const;
  const_iterator end() const;
private:

  enum class tag {
    STRING,
    APPEND
  };

  struct append {
    append(t&& lhs, 
           t&& rhs);

    std::unique_ptr<t> _lhs; 
    std::unique_ptr<t> _rhs;
  };

  union {
    std::string _string; 
    append      _append;
  };
  
  tag _tag;
  std::size_t _size;
  std::size_t _height;
};

class iterator 
  : public std::iterator<std::bidirectional_iterator_tag, char> {
    
public:
  
  iterator(); 
  iterator(t* rope_ptr, 
           std::size_t index); 
  iterator(iterator const& copy);

  reference operator*();
  iterator& operator++(); 
  iterator  operator++(int); 

  bool operator==(iterator const& rhs) const; 
  bool operator!=(iterator const& rhs) const; 

private:
  t* _ptr;
  std::size_t _index;
};

class const_iterator 
  : public std::iterator<std::bidirectional_iterator_tag, char const> {
    
public:
  
  const_iterator(); 
  const_iterator(t const * rope_ptr, 
                 std::size_t index); 
  const_iterator(const_iterator const& copy);

  value_type      operator*();
  const_iterator& operator++(); 
  const_iterator  operator++(int); 

  bool operator==(const_iterator const& rhs) const; 
  bool operator!=(const_iterator const& rhs) const; 

private:
  t const * _ptr;
  std::size_t _index;
};

} // namespace rope

#endif
