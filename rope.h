#ifndef __MAXMM__ROPE__H__
#define __MAXMM__ROPE__H__

#include <string>
#include <memory>
#include <iterator>

namespace maxmm {

class rope; 
class iterator;
class const_iterator;

// @brief Rope Class which implements basic functionality 
// of the Rope data structure for std::string. 
//
// https://en.wikipedia.org/wiki/Rope_(data_structure) 
class rope {

public:

  // Creators
  // --------

  // @brief Initialize the maxmm by moving a string.
  //
  // Most recommended constructor since no memory allocation
  // will be performed.
  explicit rope(std::string&& s);

  // @brief Initialize the maxmm by copying a string. 
  //
  // A memory allocation + copy is done for the length 
  // of the string [s].
  explicit rope(std::string const& s);

  // @bief Initialize the maxmm to be an append node 
  // of 2 sub maxmms.
  rope(rope&& lhs, 
       rope&& rhs);

  // @brief Move Constructor
  rope(rope&& copy);

  // Destructors 
  // -----------

  // @brief Destructor
  ~rope();

  // Accessors
  // ---------

  // @brief total size of the maxmm string 
  //
  // It's the sum of all the sub strings (ie leaves). 
  std::size_t size() const;

  // @brief return the height of the maxmm. 
  // - Internal height of the maxmm tree data structure
  // - Only used for testing
  std::size_t height() const;

  // @brief non const character accessor.
  char& operator[](std::size_t index);
  
  // @brief const character accessor.
  char  operator[](std::size_t index) const;

  // Modifiers
  // ---------
  
  // @brief move and append a string to the maxmm. 
  rope& append_string(std::string&& rhs); 
  
  // @brief append a string to the maxmm. 
  //
  // allocation+copy will be made.
  rope& append_string(std::string const& rhs); 

  // Iterators
  // ---------
  
  // @brief iterator to the begining of the maxmm. 
  iterator begin(); 

  // @brief iterator to the end of the maxmm. 
  //
  // dereferencing this iterator value is undefined
  // behavior.
  iterator end(); 

  // @brief const iterator to the begining of the maxmm. 
  const_iterator begin() const;

  // @brief const iterator to the end of the maxmm. 
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
    append(rope&& lhs, 
           rope&& rhs, 
           rope*  self);

    append(append&& copy, 
           rope* self);

    append(append&& copy) = delete;
    append(append const& copy) = delete;
    append() = delete;
      // Disabled constructor

    std::unique_ptr<rope> lhs_; 
    std::unique_ptr<rope> rhs_;
  };

  union {
    std::string string_; 
    append      append_;
  };

private:

  // @brief return the left most string of the maxmm data structure. 
  rope* left_most_string();

  // @brief return the next string when traversing the maxmm in a depth
  // first left to right manner. 
  //
  // @return nullptr if no more leaf string 
  // nodes
  rope* next_string_leaf(); 

  // @brief Check data structure invariant
  bool check_invariant() const;
  
  struct assert_invariant {
    assert_invariant(rope const&);
    ~assert_invariant();
    rope const& rope_;
  };
  
private:

  tag         tag_;
  std::size_t size_;
  std::size_t height_;
  rope*       parent_; // nullptr if Root node.
};

class iterator 
  : public std::iterator<std::bidirectional_iterator_tag, char> {
    
public:
  
  iterator(); 
  iterator(rope* maxmm_ptr, 
           std::size_t index); 
  iterator(iterator const& copy);

  reference operator*();
  iterator& operator++(); 
  iterator  operator++(int); 

  bool operator==(iterator const& rhs) const; 
  bool operator!=(iterator const& rhs) const; 

private:
  rope* _ptr;
  std::size_t _index;
};

class const_iterator 
  : public std::iterator<std::bidirectional_iterator_tag, char const> {
    
public:
  
  const_iterator(); 
  const_iterator(rope const * maxmm_ptr, 
                 std::size_t index); 
  const_iterator(const_iterator const& copy);

  value_type      operator*();
  const_iterator& operator++(); 
  const_iterator  operator++(int); 

  bool operator==(const_iterator const& rhs) const; 
  bool operator!=(const_iterator const& rhs) const; 

private:
  ::maxmm::iterator _i;
};

} // namespace maxmm

#endif
