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

  // @brief Initialize the rope by moving a string.
  //
  // Most recommended constructor since no memory allocation
  // will be performed.
  explicit t(std::string&& s);

  // @brief Initialize the rope by copying a string. 
  //
  // A memory allocation + copy is done for the length 
  // of the string [s].
  explicit t(std::string const& s);

  // @bief Initialize the rope to be an append node 
  // of 2 sub ropes.
  t(t&& lhs, 
    t&& rhs);

  // @brief Move Constructor
  t(t&& copy);

  // Destructors 
  // -----------

  // @brief Destructor
  ~t();

  // Accessors
  // ---------

  // @brief total size of the rope string 
  //
  // It's the sum of all the sub strings (ie leaves). 
  std::size_t size() const;

  // @brief return the height of the rope. 
  // - Internal height of the rope tree data structure
  // - Only used for testing
  std::size_t height() const;

  // @brief non const character accessor.
  char& operator[](std::size_t index);
  
  // @brief const character accessor.
  char  operator[](std::size_t index) const;

  // Modifiers
  // ---------
  
  // @brief move and append a string to the rope. 
  t& append_string(std::string&& rhs); 
  
  // @brief append a string to the rope. 
  //
  // allocation+copy will be made.
  t& append_string(std::string const& rhs); 

  // Iterators
  // ---------
  
  // @brief iterator to the begining of the rope. 
  iterator begin(); 

  // @brief iterator to the end of the rope. 
  //
  // dereferencing this iterator value is undefined
  // behavior.
  iterator end(); 

  // @brief const iterator to the begining of the rope. 
  const_iterator begin() const;

  // @brief const iterator to the end of the rope. 
  //
  // dereferencing this iterator value is undefined
  // behavior.
  const_iterator end() const;

private:

  friend class iterator;

private:

  enum class tag {
    STRING,
    APPEND
  };

  struct append {
    append(t&& lhs, 
           t&& rhs, 
           t*  self);

    append(append&& copy, 
           t* self);

    std::unique_ptr<t> _lhs; 
    std::unique_ptr<t> _rhs;
  };

  union {
    std::string _string; 
    append      _append;
  };

private:

  // @brief return the left most string of the rope data structure. 
  t* left_most_string();

  // @brief return the next string when traversing the rope in a depth
  // first left to right manner. 
  //
  // @return nullptr if no more leaf string 
  // nodes
  t* next_string_leaf(); 
  
private:

  tag         tag_;
  std::size_t size_;
  std::size_t height_;
  t*          parent_; // nullptr if Root node.
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
  ::rope::iterator _i;
};

} // namespace rope

#endif
