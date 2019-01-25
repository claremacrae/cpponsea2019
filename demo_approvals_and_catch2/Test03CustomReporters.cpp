#include "ApprovalTests.hpp"
#include "Catch.hpp"

TEST_CASE("UseSpecificReporter")
{
    Approvals::verify(
        "Some\nMulti-line\noutput",
        Windows::AraxisMergeReporter());
}

TEST_CASE("UseCustomReporter")
{
    Approvals::verify(
        "Some\nMulti-line\noutput",
        GenericDiffReporter(R"(C:\Program Files\TortoiseHg\lib\kdiff3.exe)"));
}
