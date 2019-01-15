#include "ApprovalTests.hpp"
#include "Catch.hpp"

#include "../include/ConsoleDiffReporter.h"

TEST_CASE("UseConsoleReporter")
{
    FirstWorkingReporter diff_reporter(
        {
            // On Windows, SystemLauncher does not yet know how to use the "where" command to find
            // the location of a program that is in the PATH, so we have to give the full
            // path to fc.exe here:
            new ConsoleDiffReporter("C:\\Windows\\System32\\fc.exe"),
            // On Linux and Mac, this one will be used:
            new ConsoleDiffReporter("diff")
        });
    Approvals::verify("Some\nMulti-line\noutput", diff_reporter);
}
