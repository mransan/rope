#include "rope.h"

#include <iostream>
#include <cassert>
#include <algorithm>

int main () {
  rope::t s1(std::string("123"));
  assert(s1.size() == 3);
  assert(s1[0] == '1');
  assert(s1[1] == '2');
  assert(s1[2] == '3');
  
  rope::t s2(std::string("456"));
  assert(s2.size() == 3);
  assert(s2[0] == '4');
  assert(s2[1] == '5');
  assert(s2[2] == '6');

  rope::t s3(std::move(s1), std::move(s2));
  assert(s3.size() == 6);
  assert(s3[0] == '1');
  assert(s3[1] == '2');
  assert(s3[2] == '3');
  assert(s3[3] == '4');
  assert(s3[4] == '5');
  assert(s3[5] == '6');

  s3.append_string(std::string("789"));
  assert(s3.size() == 9);
  assert(s3[0] == '1');
  assert(s3[3] == '4');
  assert(s3[6] == '7');
  assert(s3[7] == '8');
  assert(s3[8] == '9');

  rope::t s4(std::string("a")); 

  assert(s4.height() == 1); 

  s4.append_string(std::string("b"));
  assert(s4.height() == 2); 

  s4.append_string(std::string("c"));
  assert(s4.height() == 3); 

  s4.append_string(std::string("d"));
  assert(s4.height() == 3); 

  s4.append_string(std::string("e"))
    .append_string(std::string("f"))
    .append_string(std::string("g"))
    .append_string(std::string("h"));

  assert(s4.size() == 8);
  assert(s4[0] == 'a');
  assert(s4[1] == 'b');
  assert(s4[2] == 'c');
  assert(s4[3] == 'd');
  assert(s4[4] == 'e');
  assert(s4[5] == 'f');
  assert(s4[6] == 'g');
  assert(s4[7] == 'h');


  rope::t s5(std::string("ab")); 
  s5[1] = 'a';
  assert('a' == s5[1]);
  s5[1] = 'b';

  rope::iterator i = s5.begin(); 
  assert('a' == *i);
  assert('b' == *++i);

  std::string s5_str;
  std::for_each(s5.begin(), 
                s5.end(), 
                [&s5_str](rope::iterator::reference c) {
                  s5_str += c;
                });
  assert(s5_str == "ab");

  std::string s6_str;
  rope::t const s6(rope::t(std::string("ab")), 
                   rope::t(std::string("cd"))); 
  std::for_each(s6.begin(), 
                s6.end(), 
                [&s6_str](rope::const_iterator::value_type c) {
                  s6_str += c;
                });
  assert("abcd" == s6_str);

  return 0;
}
