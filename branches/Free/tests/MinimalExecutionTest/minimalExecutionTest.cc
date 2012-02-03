// C++ Minimal execution test
 
#include <iostream>
 
class test {

  public:
    test() { 
      std::cout << "Constructor Minimal execution test" << std::endl;
    };
};
 
int main()
{
  test a;
  std::cout << "Minimal execution test!";
  return 0;
}
