#include <ccel.hpp>
#include <iostream>

class test{
public:
  test(){
    a=2;
    b=3;
    c=4;
  }
  ~test();
private:
  int a,b,c;
};
int main(){
  ccel::handler_pool<test> _test_pool();
  std::cout<<"No errors!"<<std::endl;

}
