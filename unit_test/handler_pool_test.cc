#include <ccel/impl/common.hpp>
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

class test_handler_class{
public:
  test_handler_class(int a=3):_value(a){};
  void set_value(int a){_value = a;}
  const int get_value() noexcept{return _value;}
private:
  int _value;
};
BOOST_AUTO_TEST_CASE( ccel_handler_pool_test)
{
  ccel::handler_pool<test_handler_class> pool1;
  auto a=pool1.construct(1);
  BOOST_CHECK(a != nullptr);
  BOOST_TEST_MESSAGE("Address of a:"<<a);
  BOOST_REQUIRE(a->get_value() == 1);
}
