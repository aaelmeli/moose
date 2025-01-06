# This file is configured by CMake automatically as DartConfiguration.tcl
# If you choose not to use CMake, this file may be hand configured, by
# filling in the required variables.


# Configuration directories and files
SourceDirectory: /home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp
BuildDirectory: /home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp/build

# Where to place the cost data store
CostDataFile: 

# Site is something like machine.domain, i.e. pragmatic.crd
Site: sawtooth1

# Build name is osname-revision-compiler, i.e. Linux-2.4.2-2smp-c++
BuildName: Linux-mpicxx

# Subprojects
LabelsForSubprojects: testframework;wasp

# Submission information
SubmitURL: http://ci.ornl.gov/CDash/submit.php?project=wasp
SubmitInactivityTimeout: 

# Dashboard start time
NightlyStartTime: 02:00:00 UTC

# Commands for the build/test/submit cycle
ConfigureCommand: "/apps/spack/opt/gcc-12.3.0/cmake-3.27.7-5cfktbfycm7h3l77524abhbkxakzpjyn/bin/cmake" "/home/elmeabde/sawtooth1/projects/moose/framework/contrib/wasp"
MakeCommand: /apps/spack/opt/gcc-12.3.0/cmake-3.27.7-5cfktbfycm7h3l77524abhbkxakzpjyn/bin/cmake --build . --config "${CTEST_CONFIGURATION_TYPE}"
DefaultCTestConfigurationType: Release

# version control
UpdateVersionOnly: 

# CVS options
# Default is "-d -P -A"
CVSCommand: 
CVSUpdateOptions: 

# Subversion options
SVNCommand: 
SVNOptions: 
SVNUpdateOptions: 

# Git options
GITCommand: /usr/bin/git
GITInitSubmodules: 
GITUpdateOptions: 
GITUpdateCustom: 

# Perforce options
P4Command: 
P4Client: 
P4Options: 
P4UpdateOptions: 
P4UpdateCustom: 

# Generic update command
UpdateCommand: /usr/bin/git
UpdateOptions: 
UpdateType: git

# Compiler info
Compiler: /apps/local/mvapich2/2.3.7-gcc-12.3.0/bin/mpicxx
CompilerVersion: 12.3.0

# Dynamic analysis (MemCheck)
PurifyCommand: 
ValgrindCommand: 
ValgrindCommandOptions: 
DrMemoryCommand: 
DrMemoryCommandOptions: 
CudaSanitizerCommand: 
CudaSanitizerCommandOptions: 
MemoryCheckType: 
MemoryCheckSanitizerOptions: 
MemoryCheckCommand: MEMORYCHECK_COMMAND-NOTFOUND
MemoryCheckCommandOptions: 
MemoryCheckSuppressionFile: 

# Coverage
CoverageCommand: /apps/spack/opt/gcc-10.5.0/gcc-12.3.0-vx2ffftzrhvsqiflq3hwu2nissbw5dpf/bin/gcov
CoverageExtraFlags: -l

# Testing options
# TimeOut is the amount of time in seconds to wait for processes
# to complete during testing.  After TimeOut seconds, the
# process will be summarily terminated.
# Currently set to 25 minutes
TimeOut: 1500

# During parallel testing CTest will not start a new test if doing
# so would cause the system load to exceed this value.
TestLoad: 

UseLaunchers: 
CurlOptions: 
# warning, if you add new options here that have to do with submit,
# you have to update cmCTestSubmitCommand.cxx

# For CTest submissions that timeout, these options
# specify behavior for retrying the submission
CTestSubmitRetryDelay: 5
CTestSubmitRetryCount: 3
