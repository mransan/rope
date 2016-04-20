#include <string>
#include <memory>

namespace rope {
class t; 

class t {

public:

  t(std::string&& s);

  t(std::unique_ptr<t> lhs, 
    std::unique_ptr<t> rhs);

  ~t();

private:

  enum class tag {
      STRING
     ,APPEND
  };

  struct append {
      append(std::unique_ptr<t> lhs, 
             std::unique_ptr<t> rhs);

      std::unique_ptr<t> _lhs; 
      std::unique_ptr<t> _rhs;
  };

  union {
      std::string _string; 
      append _append;
  };
  
  tag _tag;
};

} // namespace rope
