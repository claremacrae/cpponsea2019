#include "ApprovalTests.hpp"
#include "Catch.hpp"

namespace
{
    // Based on SystemLauncher, and differs in that it runs the command in
    // the foreground instead of the background, so that any text output is
    // interleaved in with the output from the test framework.
    class ForegroundSystemLauncher : public CommandLauncher
    {
    public:
        bool Launch(std::vector<std::string> argv) override
        {
            SystemLauncher temp_launcher;
            if (!temp_launcher.exists(argv.front()))
            {
                return false;
            }

            // Surround each of the arguments by double-quotes:
            const std::string command = std::accumulate(
                argv.begin(), argv.end(), std::string(""),
                [](std::string a, std::string b) {return a + " " + "\"" + b + "\""; });

            // See https://stackoverflow.com/a/9965141/104370 for why the Windows string is so complex:
            const std::string launch = SystemUtils::isWindowsOs() ?
                                       (std::string("cmd /S /C ") + "\"" + command + "\"") :
                                       (command);
            system(launch.c_str());
            return true;
        }
    };

    // Based on GenericDiffReporter, but using a different launcher
    class ConsoleDiffReporter : public CommandReporter {
    private:
        ForegroundSystemLauncher launcher;

    public:
        explicit ConsoleDiffReporter(const std::string& program) : CommandReporter(program, &launcher) {};
        explicit ConsoleDiffReporter(const DiffInfo& info) : CommandReporter(info.getProgramForOs(), &launcher) {};
    };
}

TEST_CASE("UseConsoleReporter")
{
    // On Windows, SystemLauncher does not yet know how to use the "where" command to find
    // the location of a program that is in the PATH, so we have to give the full
    // path to fc.exe here:
    Approvals::verify("Some\nMulti-line\noutput", ConsoleDiffReporter("C:\\Windows\\System32\\fc.exe"));
}
