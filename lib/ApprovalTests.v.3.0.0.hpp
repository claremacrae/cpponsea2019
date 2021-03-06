#include <string>
#include <fstream>
#include <sys/stat.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <numeric>
#include <memory>
#include <stack>
#include <sstream>
#include <exception>
#include <map>
#include <functional>
#include <ostream>
 // ******************** From: ApprovalWriter.h
#ifndef APPROVALTESTS_CPP_APPROVALWRITER_H
#define APPROVALTESTS_CPP_APPROVALWRITER_H

class ApprovalWriter
{
public:
    virtual std::string getFileExtensionWithDot() = 0;
    virtual void write(std::string path) = 0;
    virtual void cleanUpReceived(std::string receivedPath) = 0;
};

#endif 

 // ******************** From: StringWriter.h
#ifndef STRINGWRITER_H
#define STRINGWRITER_H


class StringWriter : public ApprovalWriter
{
private:
    std::string s;
    std::string ext;

public:
    StringWriter( std::string contents, std::string fileExtensionWithDot = ".txt" )
        : s( contents ), ext( fileExtensionWithDot ) {}

    std::string getFileExtensionWithDot() override
    {
        return ext;
    }

    void write( std::string path ) override
    {
        std::ofstream out( path.c_str(), std::ofstream::out );
        this->Write( out );
        out.close();
    }

    void Write( std::ostream &out )
    {
        out << s << "\n";
    }

    virtual void cleanUpReceived(std::string receivedPath) override {
        remove(receivedPath.c_str());
    }


};
#endif

 // ******************** From: FileUtils.h




#ifndef CATCHPLAYGROUND_FILEUTILS_H
#define CATCHPLAYGROUND_FILEUTILS_H


class FileUtils {
public:
    static bool fileExists(std::string path) {
        return fileSize(path) != -1;
    }

    static int fileSize(std::string path) {
        struct stat statbuf;
        int stat_ok = stat(path.c_str(), &statbuf);

        if (stat_ok == -1) {
            return -1;
        }

        return int(statbuf.st_size);
    }

    static void ensureFileExists(std::string fullFilePath) {
        if (!fileExists(fullFilePath)) {
            StringWriter s("", "");
            s.write(fullFilePath);
        }
    }

    static std::string getExtensionWithDot(std::string filePath) {
        std::size_t found = filePath.find_last_of(".");
        return filePath.substr(found);
    }

    static void writeToFile(std::string filePath, std::string content)
    {
        std::ofstream out(filePath.c_str(), std::ios::binary | std::ofstream::out);
        out << content;
    }
};

#endif 

 // ******************** From: ExistingFile.h
#ifndef APPROVALTESTS_CPP_EXISTINGFILE_H
#define APPROVALTESTS_CPP_EXISTINGFILE_H



class ExistingFile : public ApprovalWriter{
    std::string filePath;
public:
    ExistingFile(std::string filePath) : filePath(filePath){}
    virtual std::string getFileExtensionWithDot() override {
        return FileUtils::getExtensionWithDot(filePath);
    }
    virtual void write(std::string path) override {
        
    }
    virtual void cleanUpReceived(std::string receivedPath) override {
        
    }
};
#endif

 // ******************** From: StringUtils.h


#ifndef APPROVALTESTS_CPP_STRINGUTILS_H
#define APPROVALTESTS_CPP_STRINGUTILS_H


class StringUtils
{
public:
    static std::string replaceAll(std::string inText, const std::string& find, const std::string& replaceWith) {
        size_t start_pos = 0;
        while ((start_pos = inText.find(find, start_pos)) != std::string::npos) {
            inText.replace(start_pos, find.length(), replaceWith);
            start_pos += replaceWith.length(); 
        }
        return inText;
    }

    static bool contains(std::string inText, const std::string& find)
    {
        return inText.find(find, 0) != std::string::npos;
    }

    static std::string toLower(std::string inText)
    {
        std::string copy(inText);
        std::transform(inText.begin(), inText.end(), copy.begin(), ::tolower);
        return copy;
    }
};
#endif 

 // ******************** From: SystemUtils.h
#ifndef SYSTEMUTILS_H
#define SYSTEMUTILS_H
// <SingleHpp unalterable>
#ifdef _WIN32
    // ReSharper disable once CppUnusedIncludeDirective
    #include <io.h>
    #include <windows.h>
#else
    // ReSharper disable once CppUnusedIncludeDirective
    #include <unistd.h>
#endif
// </SingleHpp>


class SystemUtils
{
public:
    static bool isWindowsOs()
    {
#ifdef _WIN32
        return true;
#else
        return false;
#endif

    }

    static std::string getDirectorySeparator()
    {
        return isWindowsOs() ? "\\" : "/";
    }

    
    static std::string checkFilenameCase(const std::string& fullPath)
    {
        if (!isWindowsOs() || !FileUtils::fileExists(fullPath))
        {
            return fullPath;
        }
#ifdef _WIN32

        WIN32_FIND_DATAA findFileData;
        HANDLE hFind = FindFirstFileA(fullPath.c_str(), &findFileData);

        if (hFind != INVALID_HANDLE_VALUE)
        {
            const std::string fixedFilename = findFileData.cFileName;
            const std::string fixedPath = 
                StringUtils::replaceAll( fullPath, StringUtils::toLower(fixedFilename), fixedFilename );
            FindClose(hFind);
            return fixedPath;
        }


#endif
        return fullPath;

    }
};
#endif

 // ******************** From: CommandLauncher.h
#ifndef COMMANDLAUNCHER_H
#define COMMANDLAUNCHER_H



class CommandLauncher
{
public:
    virtual ~CommandLauncher() {}
    virtual bool launch(std::vector<std::string> argv) = 0;
};

#endif  

 // ******************** From: SystemLauncher.h

#ifndef APPROVALTESTS_CPP_SYSTEMLAUNCHER_H
#define APPROVALTESTS_CPP_SYSTEMLAUNCHER_H


class SystemLauncher : public CommandLauncher
{
public:
    bool exists(const std::string& command)
    {
        bool foundByWhich = false;
        if (!SystemUtils::isWindowsOs()) {
            std::string which = "which " + command + " > /dev/null 2>&1";
            int result = system(which.c_str());
            foundByWhich = (result == 0);
        }
        return  foundByWhich || FileUtils::fileExists(command);

    }

    bool launch(std::vector<std::string> argv) override
    {
        if (!exists(argv.front()))
        {
            return false;
        }

        std::string command = std::accumulate(argv.begin(), argv.end(), std::string(""), [](std::string a, std::string b) {return a + " " + "\"" + b + "\""; });
        std::string launch = SystemUtils::isWindowsOs() ? ("start \"\" " +  command) :  (command + " &");
        system(launch.c_str());
        return true;
    }
};

#endif 

 // ******************** From: Reporter.h
#ifndef REPORTER_H
#define REPORTER_H


class Reporter {
public:
    virtual ~Reporter() = default;
    virtual bool report(std::string received, std::string approved) const = 0;
};

#endif

 // ******************** From: CommandReporter.h
#ifndef APPROVALTESTS_CPP_COMMANDREPORTER_H
#define APPROVALTESTS_CPP_COMMANDREPORTER_H



class CommandReporter : public Reporter {
private:
    std::string cmd;
    CommandLauncher *l;

protected:
    CommandReporter(std::string command, CommandLauncher *launcher)
            : cmd(command), l(launcher) {
    }

public:
    bool report(std::string received, std::string approved) const override {
        FileUtils::ensureFileExists(approved);
        std::vector<std::string> fullCommand;
        fullCommand.push_back(cmd);
        fullCommand.push_back(received);
        fullCommand.push_back(approved);
        return l->launch(fullCommand);
    }
};
#endif 

 // ******************** From: DiffInfo.h
#ifndef APPROVALTESTS_CPP_DIFFINFO_H
#define APPROVALTESTS_CPP_DIFFINFO_H


enum class Type { TEXT, IMAGE, TEXT_AND_IMAGE };



struct DiffInfo
{
    DiffInfo(std::string program, Type type) :
        program(program),
        arguments("%s %s"),
        type(type)
    {
    }
    DiffInfo(std::string program, std::string arguments, Type type) :
        program(program),
        arguments(arguments),
        type(type)
    {
    }
    std::string program;
    std::string arguments;
    Type type;

    std::string getProgramForOs() const
    {
       
        std::string result = program;
        if (result.rfind("{ProgramFiles}", 0) == 0) {
            auto result1 = StringUtils::replaceAll(result, "{ProgramFiles}", "c:\\Program Files\\");
       
            if (FileUtils::fileExists(result1)) {
                result = result1;
            }
            auto result2 = StringUtils::replaceAll(result, "{ProgramFiles}", "c:\\Program Files (x86)\\");
            if (FileUtils::fileExists(result2)) {
                result =  result2;
            }
        }
        return result;
    }
};


#endif 

 // ******************** From: DiffPrograms.h
#ifndef APPROVALTESTS_CPP_DIFFPROGRAMS_H
#define APPROVALTESTS_CPP_DIFFPROGRAMS_H



#define ENTRY(name, defaultValue) \
        static DiffInfo name() { return defaultValue; }


namespace DiffPrograms {


    namespace Mac {
        ENTRY(DIFF_MERGE,
              DiffInfo("/Applications/DiffMerge.app/Contents/MacOS/DiffMerge", "%s %s -nosplash", Type::TEXT))

        ENTRY(BEYOND_COMPARE, DiffInfo("/Applications/Beyond Compare.app/Contents/MacOS/bcomp", Type::TEXT))

        ENTRY(KALEIDOSCOPE, DiffInfo("/Applications/Kaleidoscope.app/Contents/MacOS/ksdiff", Type::TEXT_AND_IMAGE))

        ENTRY(KDIFF3, DiffInfo("/Applications/kdiff3.app/Contents/MacOS/kdiff3", "%s %s -m", Type::TEXT))

        ENTRY(P4MERGE, DiffInfo("/Applications/p4merge.app/Contents/MacOS/p4merge", Type::TEXT_AND_IMAGE))

        ENTRY(TK_DIFF, DiffInfo("/Applications/TkDiff.app/Contents/MacOS/tkdiff", Type::TEXT))

        ENTRY(VS_CODE, DiffInfo("/Applications/Visual Studio Code.app/Contents/Resources/app/bin/code", "-d %s %s", Type::TEXT))
    }
    namespace Linux {
        
        ENTRY(KDIFF3, DiffInfo("kdiff3", Type::TEXT))

        ENTRY(MELD, DiffInfo("meld", Type::TEXT))
    }
    namespace Windows {
        ENTRY(BEYOND_COMPARE_3, DiffInfo("{ProgramFiles}Beyond Compare 3\\BCompare.exe", Type::TEXT_AND_IMAGE))

        ENTRY(BEYOND_COMPARE_4, DiffInfo("{ProgramFiles}Beyond Compare 4\\BCompare.exe", Type::TEXT_AND_IMAGE))

        ENTRY(TORTOISE_IMAGE_DIFF,
              DiffInfo("{ProgramFiles}TortoiseSVN\\bin\\TortoiseIDiff.exe", "/left:%s /right:%s", Type::IMAGE))

        ENTRY(TORTOISE_TEXT_DIFF, DiffInfo("{ProgramFiles}TortoiseSVN\\bin\\TortoiseMerge.exe", Type::TEXT))

        ENTRY(WIN_MERGE_REPORTER, DiffInfo("{ProgramFiles}WinMerge\\WinMergeU.exe", Type::TEXT))

        ENTRY(ARAXIS_MERGE, DiffInfo("{ProgramFiles}Araxis\\Araxis Merge\\Compare.exe", Type::TEXT_AND_IMAGE))

        ENTRY(CODE_COMPARE, DiffInfo("{ProgramFiles}Devart\\Code Compare\\CodeCompare.exe", Type::TEXT))

        ENTRY(KDIFF3, DiffInfo("{ProgramFiles}KDiff3\\kdiff3.exe", Type::TEXT))
        ENTRY(VS_CODE, DiffInfo("{ProgramFiles}Microsoft VS Code\\Code.exe", "-d %s %s", Type::TEXT))

    }
}

#endif 

 // ******************** From: GenericDiffReporter.h
#ifndef APPROVALTESTS_CPP_GENERICDIFFREPORTER_H
#define APPROVALTESTS_CPP_GENERICDIFFREPORTER_H


class GenericDiffReporter : public CommandReporter {
private:
    SystemLauncher launcher;

public:
    GenericDiffReporter(const std::string& program) : CommandReporter(program, &launcher) {};
    GenericDiffReporter(const DiffInfo& info) : CommandReporter(info.getProgramForOs().c_str(), &launcher) {};
};

#endif 

 // ******************** From: FirstWorkingReporter.h
#ifndef CATCHPLAYGROUND_FIRSTWORKINGREPORTER_H
#define CATCHPLAYGROUND_FIRSTWORKINGREPORTER_H


class FirstWorkingReporter : public Reporter
{
private:
    std::vector< std::unique_ptr<Reporter> > reporters;
public:
    
    FirstWorkingReporter(std::vector<Reporter*> theReporters)
    {
        for(auto r : theReporters)
        {
            reporters.push_back(std::unique_ptr<Reporter>(r));
        }
    }

    bool report(std::string received, std::string approved) const override
    {
        for(auto& r : reporters)
        {
            if (r->report(received, approved))
            {
                return true;
            }
        }
        return false;
    }
};

#endif 

 // ******************** From: WindowsReporters.h
#ifndef APPROVALTESTS_CPP_WINDOWSREPORTERS_H
#define APPROVALTESTS_CPP_WINDOWSREPORTERS_H


namespace Windows {
    class BeyondCompare3Reporter : public GenericDiffReporter {
    public:
        BeyondCompare3Reporter() : GenericDiffReporter(DiffPrograms::Windows::BEYOND_COMPARE_3()) {};
    };

  class VisualStudioCodeReporter : public GenericDiffReporter {
    public:
      VisualStudioCodeReporter() : GenericDiffReporter(DiffPrograms::Windows::VS_CODE()) {};
    };

    class BeyondCompare4Reporter : public GenericDiffReporter {
    public:
        BeyondCompare4Reporter() : GenericDiffReporter(DiffPrograms::Windows::BEYOND_COMPARE_4()) {};
    };

    class BeyondCompareReporter : public FirstWorkingReporter {
    public:
        BeyondCompareReporter() : FirstWorkingReporter({new BeyondCompare4Reporter(), new BeyondCompare3Reporter()}) {
        }
    };

    class TortoiseImageDiffReporter : public GenericDiffReporter {
    public:
        TortoiseImageDiffReporter() : GenericDiffReporter(DiffPrograms::Windows::TORTOISE_IMAGE_DIFF()) {};
    };

    class TortoiseTextDiffReporter : public GenericDiffReporter {
    public:
        TortoiseTextDiffReporter() : GenericDiffReporter(DiffPrograms::Windows::TORTOISE_TEXT_DIFF()) {};
    };

    class TortoiseDiffReporter : public FirstWorkingReporter {
    public:
        TortoiseDiffReporter() : FirstWorkingReporter(
                {new TortoiseImageDiffReporter(), new TortoiseTextDiffReporter()}) {
        }
    };

    class WinMergeReporter : public GenericDiffReporter {
    public:
        WinMergeReporter() : GenericDiffReporter(DiffPrograms::Windows::WIN_MERGE_REPORTER()) {};
    };

    class AraxisMergeReporter : public GenericDiffReporter {
    public:
        AraxisMergeReporter() : GenericDiffReporter(DiffPrograms::Windows::ARAXIS_MERGE()) {};
    };

    class CodeCompareReporter : public GenericDiffReporter {
    public:
        CodeCompareReporter() : GenericDiffReporter(DiffPrograms::Windows::CODE_COMPARE()) {};
    };

    class KDiff3Reporter : public GenericDiffReporter {
    public:
        KDiff3Reporter() : GenericDiffReporter(DiffPrograms::Windows::KDIFF3()) {};
    };

    class WindowsDiffReporter : public FirstWorkingReporter {
    public:
        WindowsDiffReporter() : FirstWorkingReporter(
                {
                        new TortoiseDiffReporter(),
                        new BeyondCompareReporter(),
                        new WinMergeReporter(),
                        new AraxisMergeReporter(),
                        new CodeCompareReporter(),
                        new KDiff3Reporter(),
                        new VisualStudioCodeReporter(),
                }
        ) {
        }
    };
}

#endif 

 // ******************** From: MacReporters.h
#ifndef APPROVALTESTS_CPP_MACREPORTERS_H
#define APPROVALTESTS_CPP_MACREPORTERS_H


namespace Mac {
    class DiffMergeReporter : public GenericDiffReporter {
    public:
        DiffMergeReporter() : GenericDiffReporter(DiffPrograms::Mac::DIFF_MERGE()) {}
    };

    class VisualStudioCodeReporter : public GenericDiffReporter {
    public:
        VisualStudioCodeReporter() : GenericDiffReporter(DiffPrograms::Mac::VS_CODE()) {}
    };

    class BeyondCompareReporter : public GenericDiffReporter {
    public:
        BeyondCompareReporter() : GenericDiffReporter(DiffPrograms::Mac::BEYOND_COMPARE()) {}
    };

    class KaleidoscopeReporter : public GenericDiffReporter {
    public:
        KaleidoscopeReporter() : GenericDiffReporter(DiffPrograms::Mac::KALEIDOSCOPE()) {}
    };

    class KDiff3Reporter : public GenericDiffReporter {
    public:
        KDiff3Reporter() : GenericDiffReporter(DiffPrograms::Mac::KDIFF3()) {}
    };

    class P4MergeReporter : public GenericDiffReporter {
    public:
        P4MergeReporter() : GenericDiffReporter(DiffPrograms::Mac::P4MERGE()) {}
    };

    class TkDiffReporter : public GenericDiffReporter {
    public:
        TkDiffReporter() : GenericDiffReporter(DiffPrograms::Mac::TK_DIFF()) {}
    };

    class MacDiffReporter : public FirstWorkingReporter {
    public:
        MacDiffReporter() : FirstWorkingReporter(
                {
                        new BeyondCompareReporter(),
                        new DiffMergeReporter(),
                        new KaleidoscopeReporter(),
                        new P4MergeReporter(),
                        new KDiff3Reporter(),
                        new TkDiffReporter(),
                        new VisualStudioCodeReporter()
                }
        ) {
        }
    };
}

#endif 

 // ******************** From: LinuxReporters.h
#ifndef APPROVALTESTS_CPP_LINUXREPORTERS_H
#define APPROVALTESTS_CPP_LINUXREPORTERS_H


namespace Linux
{
    class KDiff3Reporter : public GenericDiffReporter {
    public:
        KDiff3Reporter() : GenericDiffReporter(DiffPrograms::Linux::KDIFF3()) {};
    };

    class MeldReporter : public GenericDiffReporter {
    public:
        MeldReporter() : GenericDiffReporter(DiffPrograms::Linux::MELD()) {};
    };

    class LinuxDiffReporter : public FirstWorkingReporter
    {
    public:
        LinuxDiffReporter() : FirstWorkingReporter(
                {
                        new MeldReporter(),
                        new KDiff3Reporter()
                }
        )
        {
        }
    };

}

#endif 

 // ******************** From: DiffReporter.h
#ifndef DIFFREPORTER_H
#define DIFFREPORTER_H


class DiffReporter : public FirstWorkingReporter
{
public:
    DiffReporter() : FirstWorkingReporter(
            {
                    new Mac::MacDiffReporter(),
                    new Linux::LinuxDiffReporter(),
                    new Windows::WindowsDiffReporter()
            }
    )
    {
    }
};

#endif 

 // ******************** From: Macros.h
#ifndef CATCHPLAYGROUND_MARCOS_H
#define CATCHPLAYGROUND_MARCOS_H


#define STATIC(type, name, defaultValue) \
      static type &name(type *value = NULL) { \
static type *staticValue; \
if (value != NULL) { \
staticValue = value; \
} \
if (staticValue == NULL) \
{ \
 staticValue = defaultValue; \
} \
return *staticValue; \
} \



#endif 

 // ******************** From: ApprovalNamer.h
#ifndef APPROVALNAMER_H
#define APPROVALNAMER_H


using std::string;

class ApprovalNamer
{
public:
    virtual string getApprovedFile(string extensionWithDot) = 0;
    virtual string getReceivedFile(string extensionWithDot) = 0;

};

#endif

 // ******************** From: ApprovalTestNamer.h
#ifndef APPROVALTESTNAMER_H
#define APPROVALTESTNAMER_H


using std::string;

class TestName {
public:
    const string& getFileName() const {
        return fileName;
    }

    void setFileName(const string &fileName) {
        TestName::fileName = SystemUtils::checkFilenameCase(fileName);
    }

    std::vector<string> sections;
private:
    string fileName;
};

class ApprovalTestNamer : public ApprovalNamer {
private:
public:
    ApprovalTestNamer() {
    }

    string getTestName() {
        std::stringstream ext;
        auto test = currentTest();
        for (size_t i = 0; i < test.sections.size(); i++) {
            if (0 < i) {
                ext << ".";
            }
            ext << test.sections[i];
        }

        return convertToFileName(ext.str());
    }

    static bool isForbidden(char c)
    {
        static std::string forbiddenChars("\\/:?\"<>|' ");
        return std::string::npos != forbiddenChars.find(c);
    }

    static string convertToFileName(const string& fileName)
    {
        std::stringstream result;
        for (auto ch : fileName)
        {
            if (!isForbidden(ch))
            {
                result << ch;
            }
            else
            {
                result << "_";
            }
        }
        return result.str();
    }

    string getFileName() {
        auto file = currentTest().getFileName();
        auto start = file.rfind(SystemUtils::getDirectorySeparator()) + 1;
        auto end = file.rfind(".");
        auto fileName = file.substr(start, end - start);
        return convertToFileName(fileName);
    }

    string getDirectory() {
        auto file = currentTest().getFileName();
        auto end = file.rfind(SystemUtils::getDirectorySeparator()) + 1;
        return file.substr(0, end);
    }

    STATIC(TestName, currentTest, NULL)

    virtual string getApprovedFile(string extensionWithDot) {

        return getFullFileName(".approved", extensionWithDot);
    }

    virtual string getReceivedFile(string extensionWithDot) {

        return getFullFileName(".received", extensionWithDot);
    }

    string getFullFileName(string approved, string extensionWithDot) {
        std::stringstream ext;
        ext << getDirectory() << getFileName() << "." << getTestName() << approved << extensionWithDot;
        return ext.str();
    }
};

#endif

 // ******************** From: ExistingFileNamer.h
#ifndef APPROVALTESTS_CPP_EXISTINGFILENAMER_H
#define APPROVALTESTS_CPP_EXISTINGFILENAMER_H


class ExistingFileNamer: public ApprovalNamer{
    ApprovalTestNamer namer;
    std::string filePath;
public:
    ExistingFileNamer(std::string filePath): filePath(filePath){

    }
    virtual string getApprovedFile(string extensionWithDot) {
        return namer.getApprovedFile(extensionWithDot);
    }
    virtual string getReceivedFile(string extensionWithDot) {
        return filePath;
    }

};

#endif 

 // ******************** From: ApprovalComparator.h
#ifndef APPROVALTESTS_CPP_APPROVALCOMPARATOR_H
#define APPROVALTESTS_CPP_APPROVALCOMPARATOR_H


class ApprovalComparator
{
public:
    virtual ~ApprovalComparator() = default;

    virtual bool contentsAreEquivalent(std::string receivedPath,
                                       std::string approvedPath) const = 0;
};

#endif 

 // ******************** From: TextFileComparator.h
#ifndef APPROVALTESTS_CPP_TEXTFILECOMPARATOR_H
#define APPROVALTESTS_CPP_TEXTFILECOMPARATOR_H


class TextFileComparator : public ApprovalComparator
{
public:
    static std::ifstream::int_type getNextRelevantCharacter(std::ifstream& astream)
    {
        auto ch = astream.get();
        if (ch == '\r')
        {
            return astream.get();
        }
        else
        {
            return ch;
        }
    }

    virtual bool contentsAreEquivalent(std::string receivedPath,
                                       std::string approvedPath) const
    {
        std::ifstream astream(approvedPath.c_str(),
                              std::ios::binary | std::ifstream::in);
        std::ifstream rstream(receivedPath.c_str(),
                              std::ios::binary | std::ifstream::in);

        while (astream.good() && rstream.good()) {
            int a = getNextRelevantCharacter(astream);
            int r = getNextRelevantCharacter(rstream);

            if (a != r) {
                return false;
            }
        }
        return true;
    }
};
#endif 

 // ******************** From: ApprovalException.h
#ifndef APPROVALEXCEPTION_H
#define APPROVALEXCEPTION_H


class ApprovalException : public std::exception
{
private:
    std::string message;
public:
    ApprovalException( const std::string& msg ) : message( msg ) {}

    virtual const char *what() const throw()
    {
        return message.c_str();
    }
};

class ApprovalMismatchException : public ApprovalException
{
private:
    std::string format( const std::string &received, const std::string &approved )
    {
        std::stringstream s;
        s << "Failed Approval: \n"
          << "Received does not match approved \n"
          << "Received : \"" << received << "\" \n"
          << "Approved : \"" << approved << "\"";
        return s.str();
    }
public:
    ApprovalMismatchException( std::string received, std::string approved )
        : ApprovalException( format( received, approved ) )
    {
    }
};

class ApprovalMissingException : public ApprovalException
{
private:
    std::string format( const std::string &file )
    {
        std::stringstream s;
        s << "Failed Approval: \n"
          << "Approval File Not Found \n"
          << "File: \"" << file << '"';
        return s.str();
    }
public:
    ApprovalMissingException( std::string received, std::string approved )
        : ApprovalException( format( approved ) )
    {
    }
};

#endif

 // ******************** From: FileApprover.h
#ifndef FILEAPPROVER_H
#define FILEAPPROVER_H


class FileApprover {
private:
    using ComparatorContainer = std::map< std::string, std::shared_ptr<ApprovalComparator> >;
    STATIC(ComparatorContainer, comparators, new ComparatorContainer())

public:
    FileApprover() {};

    ~FileApprover() {};

    static void registerComparator(std::string extensionWithDot, std::shared_ptr<ApprovalComparator> comparator)
    {
        comparators()[extensionWithDot] = comparator;
    }

    static std::shared_ptr<ApprovalComparator> getComparatorForFile(string receivedPath) {
        const std::string fileExtension = FileUtils::getExtensionWithDot(receivedPath);
        auto iterator = comparators().find(fileExtension);
        if (iterator != comparators().end()) {
            return iterator->second;
        }
        return std::make_shared<TextFileComparator>();
    }

    
    static void verify(std::string receivedPath,
                       std::string approvedPath,
                       const ApprovalComparator& comparator) {
        if (!FileUtils::fileExists(approvedPath)) {
            throw ApprovalMissingException(receivedPath, approvedPath);
        }

        if (!FileUtils::fileExists(receivedPath)) {
            throw ApprovalMissingException(approvedPath, receivedPath);
        }

        if (!comparator.contentsAreEquivalent(receivedPath, approvedPath)) {
            throw ApprovalMismatchException(receivedPath, approvedPath);
        }
    }

    static void verify(std::string receivedPath,
                       std::string approvedPath) {
        verify(receivedPath, approvedPath, *getComparatorForFile(receivedPath));
    }

    static void verify(ApprovalNamer& n, ApprovalWriter& s, const Reporter& r) {
        std::string approvedPath = n.getApprovedFile(s.getFileExtensionWithDot());
        std::string receivedPath = n.getReceivedFile(s.getFileExtensionWithDot());
        s.write(receivedPath);
        try
        {
            verify(receivedPath, approvedPath);
            s.cleanUpReceived(receivedPath);
        }
        catch (const ApprovalException&) {
            r.report(receivedPath, approvedPath);
            throw;
        }
    }


};

#endif

 // ******************** From: Approvals.h
#ifndef APPROVALS_H
#define APPROVALS_H


class Approvals {
private:
    Approvals() {}

    ~Approvals() {}

public:
    static void verify(std::string contents, const Reporter &reporter = DiffReporter()) {
        StringWriter writer(contents);
        ApprovalTestNamer namer;
        FileApprover::verify(namer, writer, reporter);
    }

    template<typename T>
    static void verify(T contents, const Reporter &reporter = DiffReporter()) {
        std::stringstream s;
        s << contents;
        verify(s.str(), reporter);
    }

    template<typename Iterator>
    static void verifyAll(std::string header,
                          const Iterator &start, const Iterator &finish,
                          std::function<void(typename Iterator::value_type, std::ostream &)> converter,
                          const Reporter &reporter = DiffReporter()) {
        std::stringstream s;
        if (!header.empty()) {
            s << header << "\n\n\n";
        }
        for (auto it = start; it != finish; ++it) {
            converter(*it, s);
            s << '\n';
        }
        verify(s.str(), reporter);
    }

    template<typename Container>
    static void verifyAll(std::string header,
                          const Container &list,
                          std::function<void(typename Container::value_type, std::ostream &)> converter,
                          const Reporter &reporter = DiffReporter()) {
        verifyAll<typename Container::const_iterator>(header, list.begin(), list.end(), converter, reporter);
    }

    template<typename T>
    static void verifyAll(std::string header,
                          const std::vector<T> &list,
                          const Reporter &reporter = DiffReporter()) {
        int i = 0;
        verifyAll<std::vector<T>>(header, list, [&](T e, std::ostream &s) { s << "[" << i++ << "] = " << e; },
                                  reporter);
    }

    template<typename T>
    static void verifyAll(const std::vector<T> &list,
                          const Reporter &reporter = DiffReporter()) {
        verifyAll<T>("", list, reporter);
    }

    static void verifyExistingFile(const std::string filePath, const Reporter &reporter = DiffReporter()) {
        ExistingFile writer(filePath);
        ExistingFileNamer namer(filePath);
        FileApprover::verify(namer, writer, reporter);
    }
};

#endif

 // ******************** From: Catch2Approvals.h

#ifndef CATCHPLAYGROUND_CATCH2APPROVALS_H_H
#define CATCHPLAYGROUND_CATCH2APPROVALS_H_H


// <SingleHpp unalterable>
#ifdef APPROVALS_CATCH
#define CATCH_CONFIG_MAIN

#include "Catch.hpp"

struct Catch2ApprovalListener : Catch::TestEventListenerBase {
    using TestEventListenerBase::TestEventListenerBase;
    TestName currentTest;

    Catch2ApprovalListener(Catch::ReporterConfig const &_config) : Catch::TestEventListenerBase(_config) {}

    virtual void testCaseStarting(Catch::TestCaseInfo const &testInfo) override {

        currentTest.setFileName(testInfo.lineInfo.file);
        ApprovalTestNamer::currentTest(&currentTest);
    }

    virtual void testCaseEnded(Catch::TestCaseStats const &testCaseStats) override {
        while (!currentTest.sections.empty()) {
            currentTest.sections.pop_back();
        }
    }

    virtual void sectionStarting(Catch::SectionInfo const &sectionInfo) override {
        currentTest.sections.push_back(sectionInfo.name);
    }

    virtual void sectionEnded(Catch::SectionStats const &sectionStats) override {
        currentTest.sections.pop_back();
    }
};

CATCH_REGISTER_LISTENER(Catch2ApprovalListener)

#endif
// </SingleHpp>
#endif 

 // ******************** From: CombinationApprovals.h
#ifndef COMBINATIONAPPROVALS_H
#define COMBINATIONAPPROVALS_H


class Empty
{
public:
    template< typename Other>
    bool operator!=(const Other &rhs) const {
        return true;
    }

    bool operator!=(const Empty &rhs) const {
        return false;
    }

    friend std::ostream &operator<<(std::ostream &os, const Empty &empty) {
        os << "This should never be written - see Empty\n";
        return os;
    }

};

#define EMPTY std::vector<Empty>{Empty()}

class CombinationApprovals
{
public:
    CombinationApprovals() = delete;
    ~CombinationApprovals() = delete;

    template <
            typename Container1,
            typename Container2,
            typename Container3,
            typename Container4,
            typename Container5,
            typename Container6,
            typename Container7,
            typename Container8,
            typename Container9,
            typename ReturnType>
    static void verifyAllCombinations(
                          std::function<ReturnType (
                                  typename Container1::value_type,
                                  typename Container2::value_type,
                                  typename Container3::value_type,
                                  typename Container4::value_type,
                                  typename Container5::value_type,
                                  typename Container6::value_type,
                                  typename Container7::value_type,
                                  typename Container8::value_type,
                                  typename Container9::value_type)> converter,
                          const Container1& inputs1,
                          const Container2& inputs2,
                          const Container3& inputs3,
                          const Container4& inputs4,
                          const Container5& inputs5,
                          const Container6& inputs6,
                          const Container7& inputs7,
                          const Container8& inputs8,
                          const Container9& inputs9,
                          const Reporter& reporter = DiffReporter())
    {
        Empty empty;
        std::stringstream s;
        for (auto input1 : inputs1)
        {
            for (auto input2 : inputs2)
            {
                for (auto input3 : inputs3)
                {
                    for (auto input4 : inputs4)
                    {
                        for (auto input5 : inputs5)
                        {
                            for (auto input6 : inputs6)
                            {
                                for (auto input7 : inputs7)
                                {
                                    for (auto input8 : inputs8)
                                    {
                                        for (auto input9 : inputs9)
                                        {
                                            ReturnType result = converter(input1, input2, input3, input4, input5,
                                                                          input6, input7, input8, input9);
                                            s << "(" << input1;
                                            if (empty != input2) { s << ", " << input2; }
                                            if (empty != input3) { s << ", " << input3; }
                                            if (empty != input4) { s << ", " << input4; }
                                            if (empty != input5) { s << ", " << input5; }
                                            if (empty != input6) { s << ", " << input6; }
                                            if (empty != input7) { s << ", " << input7; }
                                            if (empty != input8) { s << ", " << input8; }
                                            if (empty != input9) { s << ", " << input9; }
                                            s << ") => " << result << '\n';
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        Approvals::verify(s.str(), reporter);
    }

    template <
            typename Container1,
            typename Container2,
            typename Container3,
            typename Container4,
            typename Container5,
            typename Container6,
            typename Container7,
            typename Container8,
            typename ReturnType>
    static void verifyAllCombinations(
            std::function<ReturnType (
                    typename Container1::value_type,
                    typename Container2::value_type,
                    typename Container3::value_type,
                    typename Container4::value_type,
                    typename Container5::value_type,
                    typename Container6::value_type,
                    typename Container7::value_type,
                    typename Container8::value_type)> converter,
            const Container1& inputs1,
            const Container2& inputs2,
            const Container3& inputs3,
            const Container4& inputs4,
            const Container5& inputs5,
            const Container6& inputs6,
            const Container7& inputs7,
            const Container8& inputs8,
            const Reporter& reporter = DiffReporter())
    {
        verifyAllCombinations<
                Container1,
                Container2,
                Container3,
                Container4,
                Container5,
                Container6,
                Container7,
                Container8,
                std::vector<Empty>, ReturnType>([&](
                typename Container1::value_type i1,
                typename Container2::value_type i2,
                typename Container3::value_type i3,
                typename Container4::value_type i4,
                typename Container5::value_type i5,
                typename Container6::value_type i6,
                typename Container7::value_type i7,
                typename Container8::value_type i8,
                Empty _){return converter(i1, i2, i3, i4, i5, i6, i7, i8);},
                                              inputs1,
                                              inputs2,
                                              inputs3,
                                              inputs4,
                                              inputs5,
                                              inputs6,
                                              inputs7,
                                              inputs8,
                                              EMPTY,
                                              reporter);
    }


    template <
            typename Container1,
            typename Container2,
            typename Container3,
            typename Container4,
            typename Container5,
            typename Container6,
            typename Container7,
            typename ReturnType>
    static void verifyAllCombinations(
            std::function<ReturnType (
                    typename Container1::value_type,
                    typename Container2::value_type,
                    typename Container3::value_type,
                    typename Container4::value_type,
                    typename Container5::value_type,
                    typename Container6::value_type,
                    typename Container7::value_type)> converter,
            const Container1& inputs1,
            const Container2& inputs2,
            const Container3& inputs3,
            const Container4& inputs4,
            const Container5& inputs5,
            const Container6& inputs6,
            const Container7& inputs7,
            const Reporter& reporter = DiffReporter())
    {
        verifyAllCombinations<
                Container1,
                Container2,
                Container3,
                Container4,
                Container5,
                Container6,
                Container7,
                std::vector<Empty>, ReturnType>([&](
                typename Container1::value_type i1,
                typename Container2::value_type i2,
                typename Container3::value_type i3,
                typename Container4::value_type i4,
                typename Container5::value_type i5,
                typename Container6::value_type i6,
                typename Container7::value_type i7,
                Empty _){return converter(i1, i2, i3, i4, i5, i6, i7);},
                                              inputs1,
                                              inputs2,
                                              inputs3,
                                              inputs4,
                                              inputs5,
                                              inputs6,
                                              inputs7,
                                              EMPTY,
                                              reporter);
    }

    template <
            typename Container1,
            typename Container2,
            typename Container3,
            typename Container4,
            typename Container5,
            typename Container6,
            typename ReturnType>
    static void verifyAllCombinations(
            std::function<ReturnType (
                    typename Container1::value_type,
                    typename Container2::value_type,
                    typename Container3::value_type,
                    typename Container4::value_type,
                    typename Container5::value_type,
                    typename Container6::value_type)> converter,
            const Container1& inputs1,
            const Container2& inputs2,
            const Container3& inputs3,
            const Container4& inputs4,
            const Container5& inputs5,
            const Container6& inputs6,
            const Reporter& reporter = DiffReporter())
    {
        verifyAllCombinations<
                Container1,
                Container2,
                Container3,
                Container4,
                Container5,
                Container6,
                std::vector<Empty>, ReturnType>([&](
                typename Container1::value_type i1,
                typename Container2::value_type i2,
                typename Container3::value_type i3,
                typename Container4::value_type i4,
                typename Container5::value_type i5,
                typename Container6::value_type i6,
                Empty _){return converter(i1, i2, i3, i4, i5, i6);},
                                              inputs1,
                                              inputs2,
                                              inputs3,
                                              inputs4,
                                              inputs5,
                                              inputs6,
                                              EMPTY,
                                              reporter);
    }

    template <
            typename Container1,
            typename Container2,
            typename Container3,
            typename Container4,
            typename Container5,
            typename ReturnType>
    static void verifyAllCombinations(
            std::function<ReturnType (
                    typename Container1::value_type,
                    typename Container2::value_type,
                    typename Container3::value_type,
                    typename Container4::value_type,
                    typename Container5::value_type)> converter,
            const Container1& inputs1,
            const Container2& inputs2,
            const Container3& inputs3,
            const Container4& inputs4,
            const Container5& inputs5,
            const Reporter& reporter = DiffReporter())
    {
        verifyAllCombinations<
                Container1,
                Container2,
                Container3,
                Container4,
                Container5,
                std::vector<Empty>, ReturnType>([&](
                                                      typename Container1::value_type i1,
                                                      typename Container2::value_type i2,
                                                      typename Container3::value_type i3,
                                                      typename Container4::value_type i4,
                                                      typename Container5::value_type i5,
                                                      Empty _){return converter(i1, i2, i3, i4, i5);},
                                              inputs1,
                                              inputs2,
                                              inputs3,
                                              inputs4,
                                              inputs5,
                                              EMPTY,
                                              reporter);
    }

    template <
            typename Container1,
            typename Container2,
            typename Container3,
            typename Container4,
            typename ReturnType>
    static void verifyAllCombinations(
            std::function<ReturnType (
                    typename Container1::value_type,
                    typename Container2::value_type,
                    typename Container3::value_type,
                    typename Container4::value_type)> converter,
            const Container1& inputs1,
            const Container2& inputs2,
            const Container3& inputs3,
            const Container4& inputs4,
            const Reporter& reporter = DiffReporter())
    {
        verifyAllCombinations<
                Container1,
                Container2,
                Container3,
                Container4,
                std::vector<Empty>, ReturnType>([&](
                                                      typename Container1::value_type i1,
                                                      typename Container2::value_type i2,
                                                      typename Container3::value_type i3,
                                                      typename Container4::value_type i4,
                                                      Empty _){return converter(i1, i2, i3, i4);},
                                              inputs1,
                                              inputs2,
                                              inputs3,
                                              inputs4,
                                              EMPTY,
                                              reporter);
    }

    template <
            typename Container1,
            typename Container2,
            typename Container3,
            typename ReturnType>
    static void verifyAllCombinations(
            std::function<ReturnType (
                    typename Container1::value_type,
                    typename Container2::value_type,
                    typename Container3::value_type)> converter,
            const Container1& inputs1,
            const Container2& inputs2,
            const Container3& inputs3,
            const Reporter& reporter = DiffReporter())
    {
        verifyAllCombinations<
                Container1,
                Container2,
                Container3,
                std::vector<Empty>, ReturnType>([&](
                                                      typename Container1::value_type i1,
                                                      typename Container2::value_type i2,
                                                      typename Container3::value_type i3,
                                                      Empty _){return converter(i1, i2, i3);},
                                              inputs1,
                                              inputs2,
                                              inputs3,
                                              EMPTY,
                                              reporter);
    }

    template <
            typename Container1,
            typename Container2,
            typename ReturnType>
    static void verifyAllCombinations(
            std::function<ReturnType (
                    typename Container1::value_type,
                    typename Container2::value_type)> converter,
            const Container1& inputs1,
            const Container2& inputs2,
            const Reporter& reporter = DiffReporter())
    {
        verifyAllCombinations<
                Container1,
                Container2,
                std::vector<Empty>, ReturnType>([&](
                                                      typename Container1::value_type i1,
                                                      typename Container2::value_type i2,
                                                      Empty _){return converter(i1, i2);},
                                              inputs1,
                                              inputs2,
                                              EMPTY,
                                              reporter);
    }

    template <
            typename Container1,
            typename ReturnType>
    static void verifyAllCombinations(
            std::function<ReturnType (
                    typename Container1::value_type)> converter,
            const Container1& inputs1,
            const Reporter& reporter = DiffReporter())
    {
        verifyAllCombinations<
                Container1,
                std::vector<Empty>, ReturnType>([&](
                                                      typename Container1::value_type i1,
                                                      Empty _){return converter(i1);},
                                              inputs1,
                                              EMPTY,
                                              reporter);
    }
};

#endif

 // ******************** From: GoogleTestApprovals.h
#ifndef APPROVALTESTS_CPP_GOOGLTESTAPPPROVALS_H
#define APPROVALTESTS_CPP_GOOGLTESTAPPPROVALS_H


#ifdef APPROVALS_GOOGLETEST_EXISTING_MAIN
#define APPROVALS_GOOGLETEST
#endif

#ifdef APPROVALS_GOOGLETEST

// <SingleHpp unalterable>
#include "gtest/gtest.h"


class GoogleTestListener : public ::testing::EmptyTestEventListener
{
    TestName currentTest;
public:
    virtual void OnTestStart(const ::testing::TestInfo& testInfo) override
    {
        currentTest.setFileName(testInfo.file());
        currentTest.sections = {};
        if (! StringUtils::contains(currentTest.getFileName(), std::string(testInfo.test_case_name()) + ".") )
        {
            currentTest.sections.push_back(testInfo.test_case_name());
        }
        if (! std::string(testInfo.name()).empty())
        {
            currentTest.sections.push_back(testInfo.name());
        }
        
        ApprovalTestNamer::currentTest(&currentTest);
    }
};

void initializeApprovalTestsForGoogleTests() {
    auto& listeners = testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new GoogleTestListener);
}

#ifndef APPROVALS_GOOGLETEST_EXISTING_MAIN
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    initializeApprovalTestsForGoogleTests();
    return RUN_ALL_TESTS();
}
#endif // APPROVALS_GOOGLETEST_EXISTING_MAIN

// </SingleHpp>
#endif
#endif 

 // ******************** From: OkraApprovals.h
#ifndef APPROVALTESTS_CPP_OKRAAPPPROVALS_H
#define APPROVALTESTS_CPP_OKRAAPPPROVALS_H

#ifdef APPROVALS_OKRA

// <SingleHpp unalterable>
#include "Okra.h"


struct OkraApprovalListener : public okra::IListener
{
 TestName currentTest;

public:
 void OnStart(const okra::TestInfo &testInfo) override
 {
  currentTest.fileName = testInfo.file_path;
  currentTest.sections = {testInfo.name};
  ApprovalTestNamer::currentTest(&currentTest);
 }

 void OnEnd(const okra::TestInfo &testInfo, std::chrono::high_resolution_clock::duration duration) override {
 }
 void OnFail(const std::string &message) override {
 }
};

OKRA_REGISTER_LISTENER(OkraApprovalListener);
// </SingleHpp>
#endif
#endif 

 // ******************** From: ClipboardReporter.h
#ifndef APPROVALTESTS_CPP_COMMANDLINEREPORTER_H
#define APPROVALTESTS_CPP_COMMANDLINEREPORTER_H




class ClipboardReporter : public Reporter {
public:
    static std::string getCommandLineFor(std::string received, std::string approved, bool isWindows)
    {
        if (isWindows) {
            return std::string("move /Y ") + "\"" + received + "\" \"" + approved + "\"";
        } else {
            return std::string("mv ") + "\"" + received + "\" \"" + approved + "\"";
        }
    }

    virtual bool report(std::string received, std::string approved) const override
    {
        copyToClipboard(getCommandLineFor(received, approved, SystemUtils::isWindowsOs()));
        return true;
    }

    void copyToClipboard(const std::string& newClipboard) const {
        

        const std::string clipboardCommand = SystemUtils::isWindowsOs() ? "clip" : "pbclip";
        auto cmd = std::string("echo ") + newClipboard + " | " + clipboardCommand;
        system(cmd.c_str());
    }
};

#endif 

 // ******************** From: CombinationReporter.h
#ifndef CATCHPLAYGROUND_COMBINATIONREPORTER_H
#define CATCHPLAYGROUND_COMBINATIONREPORTER_H


class CombinationReporter : public Reporter
{
private:
    std::vector< std::unique_ptr<Reporter> > reporters;
public:
    
    CombinationReporter(std::vector<Reporter*> theReporters)
    {
        for(auto r : theReporters)
        {
            reporters.push_back(std::unique_ptr<Reporter>(r));
        }
    }

    bool report(std::string received, std::string approved) const override
    {
        bool result = false;
        for(auto& r : reporters)
        {
            result |= r->report(received, approved);
        }
        return result;
    }
};

#endif 

 // ******************** From: QuietReporter.h
#ifndef CATCHPLAYGROUND_QUIETREPORTER_H
#define CATCHPLAYGROUND_QUIETREPORTER_H



class QuietReporter : public Reporter
{
public:
    bool report(std::string received, std::string approved) const override
    {
        return true;
    }
};

#endif 

