#include "ApprovalTests.hpp"
#include "Catch.hpp"

// Approvals test - test trivial value, for demo purposes
TEST_CASE("TestFixedInput")
{
    Approvals::verify("Some\nMulti-line\noutput");
}
