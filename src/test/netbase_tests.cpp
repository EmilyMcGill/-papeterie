#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

#include "netbase.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(netbase_tests)

BOOST_AUTO_TEST_CASE(netbase_networks)
{
    BOOST_CHECK(CNetAddr("127.0.0.1").GetNetwork()                              == NET_UNROUTABLE);
    BOOST_CHECK(CNetAddr("::1").GetNetwork()                                    == NET_UNROUTABLE);
    BOOST_CHECK(CNetAddr("8.8.8.8").GetNetwork()                                == NET_IPV4);
    BOOST_CHECK(CNetAddr("2001::8888").GetNetwork()                             == NET_IPV6);
    BOOST_CHECK(CNetAddr("FD87