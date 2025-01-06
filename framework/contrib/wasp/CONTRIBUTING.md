# Topic-based branch development
A topic branch is a short-lived branch that you create for a single focus of work.
Generally, short-lived is less than 6 months. The work can be bug fixes, feature development or design testing.
It is good practice to always use a branch when introducing changes.
This allows the master to remain passing and all subsequent branches to start clean.

# Formatting
The use of the utility [clang-format](https://clang.llvm.org/docs/ClangFormat.html) makes for a quick processing of the desired format.
For a specific file, the following command will load default options for style formatting (assuming clang-format is installed an on your path).
```
clang-format -i specific_file.cpp
```
# Naming convention
## File naming
C++ files should end in `.cpp` and header files in `.h`.
Template implementations should end in `.i.h`.
Flex grammar files should use '*.lex' suffix. GNU Bison grammar files should end in '*.bison' suffix. 

## Class naming
Classes should be **CamelCase**.
```c++
namespace wasp
{
class CommandLine
{
};
}
```
Class members should be prefixed with **m_** and be lower-case snake-case **m_favorite_variable**.
```c++
namespace wasp
{
class CommandLine
{
private:
  int m_argc;
  char ** m_argv;
};
}
```
Class methods should be lower-case snake-case **push_back**.
Class member getter methods should be the members name in lower-case snake-case **favorite_variable**  `int favorite_variable() const`.
```c++
namespace wasp
{
class Commandline
{
private:
  int mArgc;
public:
  int argc() const;
  int my_method() const;
};
}
```
