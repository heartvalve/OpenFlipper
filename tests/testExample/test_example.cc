// C++ Hello World
 
#include <iostream>
 
class test {

  public:
    test() { 
      std::cout << "Constructor" << std::endl;
    };
};
 
int main()
{
  test a;
  std::cout << "Hello World!";
}
