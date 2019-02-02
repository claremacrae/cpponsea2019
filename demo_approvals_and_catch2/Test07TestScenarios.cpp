#include <ostream>
#include "ApprovalTests.hpp"
#include "Catch.hpp"

namespace
{
    class LegacyResult;
    std::ostream &operator<<(std::ostream &os, const LegacyResult &result);
    LegacyResult doLegacyOperation();

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
    const LegacyResult result = doLegacyOperation();
    Approvals::verify(result);
}

//--------------------------------------------------------------------------------

// Problem: Output contains data that changes each run, e.g. date-and-time

namespace
{
    class LogFileWriter
    {
    public:
        void write(std::string path)
        {
            std::ofstream stream(path);
            stream << "2019-02-01 19:27:34 Sample log text\n";
        }
    };

    void rewriteLogFileRemovingDatesAndTimes(std::string path)
    {
        // Implementation:
        //  1. reads the given file
        //  2. converts any data-and-time strings to some boilerplate, e.g. [date-time-removed]
        //  3. overwrites with given file with the updated content
    }

    class DateRemovingLogFileWriter : public ApprovalWriter
    {
    public:
        DateRemovingLogFileWriter(LogFileWriter& writer) : writer(writer)
        {
        }
        string getFileExtensionWithDot() override
        {
            return ".log";
        }

        void write(std::string path) override
        {
            writer.write(path);
            rewriteLogFileRemovingDatesAndTimes(path);
        }

        void cleanUpReceived(std::string receivedPath) override
        {
            ::remove(receivedPath.c_str());
        }
    private:
        LogFileWriter& writer;
    };
}

TEST_CASE("Deal with dates and times in output")
{
    // Could refactor the system being tested to allow use of fixed dates and times
    // but that risks breaking the system before you have tests. It would be
    // a useful later step.

    // Or could write a custom diff mechanism that recognises dates and times,
    // and ignores differences in them. Non-trivial.

    // Instead we will post-process our log files when running in test, and
    // strip out any dates and times, so that the files are expected to be
    // identical.

    ApprovalTestNamer namer;

    LogFileWriter writerBeingTested;
    DateRemovingLogFileWriter datelessWriter(writerBeingTested);

    FileApprover::verify(namer, datelessWriter, DiffReporter());
}

//--------------------------------------------------------------------------------

// Problem: Output is not suitable for writing to a text file

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

