#include <ostream>
#include "ApprovalTests.hpp"
#include "Catch.hpp"

namespace
{
    class LegacyResult
    {
    };

    std::ostream &operator<<(std::ostream &os, const LegacyResult &result)
    {
        os << "Some complicated output\nfrom my legacy code\n";
        return os;
    }

    LegacyResult doLegacyOperation()
    {
        return LegacyResult();
    }
}

TEST_CASE("New test of legacy feature")
{
    // Wrap up your legacy feature that needs testing in a function call
    // that returns some state that can be written to a text file
    LegacyResult result = doLegacyOperation();
    Approvals::verify(result);
}

//--------------------------------------------------------------------------------

