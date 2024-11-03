#include <exception>
#include <string>
class Error : public std::exception {
private:
  std::string message_;

public:
  Error(const std::string &message) : message_(message) {}
  const char *what() const noexcept override { return message_.c_str(); }
};