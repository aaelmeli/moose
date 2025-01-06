#!/bin/bash

#######################################################################
# File   - test_moose_app_integration.sh
# Author - Brandon Langley
# Date   - Mon Apr 12, 2021
# Brief  -
#        - This script is used to test the integration of WASP with any
#        - MOOSE-based application (https://github.com/idaholab/moose).
#        -
#        - The path to the MOOSE application, e.g., '/path/to/app-opt',
#        - must be provided as the only required command line argument.
#        -
#        - The script then searches for WASP utilities used for parsing
#        - and validating MOOSE inputs relative to the script location:
#        - 1) '../wasputils' - handling executing inside the build tree
#        - 2) '../bin' - handling executing this script from an install
#        -
#        - Next, it runs the MOOSE application using the '--definition'
#        - flag to create a schema definition to validate MOOSE inputs.
#        -
#        - Then, it follows these steps building a list of input paths:
#        - 1) Doing a verbose dry-run of the application's test harness
#        - 2) Processing the output and build input file absolute paths
#        - 3) Moving into the directory of every input file in the list
#        - 4) Running the MOOSE application --check-input on each input
#        - 5) Keeping inputs that produce 'Syntax OK' to test with WASP
#        -
#        - Finally, the WASP utilties check every file in the list for:
#        - Finally, the WASP utilities check each file in the list for:
#        - (1) parsing pass (2) identical roundtrip (3) validation pass
#        -
#        - If all three of the checks pass for every input in the list,
#        - then this integration test is deemed to pass returning true.
#        -
#        - If any of these three checks fail for any input in the list,
#        - then the integration test is deemed to fail returning false.
#        -
#        - Additionally all of the parse errors, roundtrip differences,
#        - and / or validation failures are printed to standard output.
#        -
#######################################################################

##### STORE APPLICATION PATH AND OPTIONAL TEST EXCLUSION LIST #########

if [[ $# -ne 1 ]] && [[ $# -ne 2 ]] ; then
    echo
    echo "Expected one or two command line arguments:"
    echo "  1) path to application e.g., '/path/to/app-name/app-opt'"
    echo "  2) path to file list of test input exclusions (optional)"
    echo
    exit 1
fi

TESTDIR_ABS_PATH=$(dirname "$0")

APPLICATION_PATH="$1"

if [[ $# -eq 2 ]] ; then

    if [ -f "$2" ] ; then

        ##### NOTE THAT LINES WITH # IN  FIRST COLUMN ARE IGNORED #####

        INPUT_EXCLUSION_LIST=`cat "$2" | grep -v "^#"`

    else
        echo
        echo "The provided optional input exclusion list file does not exist:"
        echo
        echo "  '$2'"
        echo
        exit 1
    fi

else

    INPUT_EXCLUSION_LIST=""

fi

##### TEMPORARY FILE PATHS TO BE WRITTEN / USED / DELETED #############

TEMP_SCHEM_FILE="${TESTDIR_ABS_PATH}/.temp-schema.sch"
TEMP_SYNTAX_DIR="${TESTDIR_ABS_PATH}/.temp-syntax-dir"
TEMP_PARSE_FILE="${TESTDIR_ABS_PATH}/.temp-parse-file"
TEMP_ROUND_FILE="${TESTDIR_ABS_PATH}/.temp-round-file"
TEMP_VALID_FILE="${TESTDIR_ABS_PATH}/.temp-valid-file"

##### FIND INPUT-ROUNDTRIP AND INPUT-VALIDATOR RELATIVE UTILITIES #####

if [[ -x "${TESTDIR_ABS_PATH}/../wasputils/hitselect" ]] && \
   [[ -x "${TESTDIR_ABS_PATH}/../wasputils/hitvalid"  ]]
then
    WASP_UTILITY_DIR="${TESTDIR_ABS_PATH}/../wasputils"
elif [[ -x "${TESTDIR_ABS_PATH}/../bin/hitselect" ]] && \
     [[ -x "${TESTDIR_ABS_PATH}/../bin/hitvalid"  ]]
then
    WASP_UTILITY_DIR="${TESTDIR_ABS_PATH}/../bin"
else
    echo
    echo "Utilities must be relative to this script in ../wasputils or ../bin"
    echo
    exit 1
fi

INPUT_ROUNDTRIP="${WASP_UTILITY_DIR}/hitselect"
INPUT_VALIDATOR="${WASP_UTILITY_DIR}/hitvalid"

##### REMOVE TEMPORARY FILES IN CASE LAST CLEANUP DID NOT HAPPEN ######

rm -rf ${TEMP_SCHEM_FILE} 2> /dev/null
rm -rf ${TEMP_SYNTAX_DIR} 2> /dev/null
rm -rf ${TEMP_PARSE_FILE} 2> /dev/null
rm -rf ${TEMP_ROUND_FILE} 2> /dev/null
rm -rf ${TEMP_VALID_FILE} 2> /dev/null

##### USE APPLICATION PATH FROM THE COMMAND LINE TO CREATE SCHEMA #####

echo
echo "Generating schema using '${APPLICATION_PATH} --definition'"
echo

${APPLICATION_PATH} --definition > ${TEMP_SCHEM_FILE}

##### COMPILE LIST OF ALL INPUTS THAT RUN THROUGH THE TEST HARNESS ####

echo
echo "Gathering all input files that run through the test harness..."
echo

cd $(dirname "${APPLICATION_PATH}")

##### This statement creates a list of every possible input file that we may use
##### in the integration testing (before futher filtering) by doing the following:
##### ./run_tests            :: runs the application's full test-harness
##### - --dry-run            :: print commands to run, but don't actually run them
##### - --verbose            :: show the output of every test
##### - --no-report          :: do not report skipped tests
##### - grep -B 1 -E "Run... :: grab the 'Running command' line which contains the file name
#####                           only if the test actually runs the app-opt -i filename.i
#####                           and one line before it which contains the 'Working Directory'
##### - grep -E -v '^\-\-$'  :: remove the '--'' separator from the previous `grep -B` call
##### - paste -d " " - -     :: combine every pair of lines ('Working Directory' line and
#####                           'Running command' line) to be a single space-delimited line
##### - sed -e "s/^.*Work... :: Combine the directory after 'Working Directory:' with the
#####                           file name from 'Running command:' to create the absolute
#####                           path to each test input file and filter out all of th rest
##### - sort | uniq | sort   :: It is possible for the same input file to be run in multiple
#####                           tests using different command line options, so this just
#####                           filters out the duplicates files and sorts for consistancy

ALL_FILES=`./run_tests --dry-run --verbose --no-report | \
    grep -B 1 -E "Running command:[' ']+.*\-opt[' ']+\-i[' ']+[a-zA-Z0-9_/\.\-]+\.i " | \
    grep -E -v '^\-\-$' | \
    paste -d " " - - | \
    sed -e "s/^.*Working Directory:[' ']*\([/a-zA-Z0-9_\-]*\).*Running command:.*\-opt[' ']*\-i[' ']*\([a-zA-Z0-9_/\.\-]*\.i\).*$/\1\/\2/" | \
    sort | uniq | sort`

cd - > /dev/null

##### FILTER LIST OF INPUT FILES TO BE EXCLUDED IF FILE EXISTS ########

echo
echo "Filtering possible list of exclusion files from the total list..."
echo

if [[ ${#INPUT_EXCLUSION_LIST[@]} -eq 0 ]] ; then

    EXCLUSION_FILTERED_FILES=`echo "${ALL_FILES}"`

else

    for FILE in ${INPUT_EXCLUSION_LIST} ; do

        COMMAND+=`echo "fgrep -v '${FILE}' | "`

    done

    COMMAND=`echo ${COMMAND} | rev | cut -c 2- | rev`

    if [[ -z ${COMMAND} ]] ; then

        EXCLUSION_FILTERED_FILES=`echo "${ALL_FILES}"`

    else

        EXCLUSION_FILTERED_FILES=`echo "${ALL_FILES}" | eval ${COMMAND}`

    fi

fi

##### INPUTS SHOULD PASS THE --check-input SYNTAX CHECK TO MOVE ON ####

echo
echo "Gathering all input files that also pass the --input-check..."
echo

for FILE in ${EXCLUSION_FILTERED_FILES} ; do

    rm -rf ${TEMP_SYNTAX_DIR} && mkdir ${TEMP_SYNTAX_DIR} && cd ${TEMP_SYNTAX_DIR}

    STATUS=`${APPLICATION_PATH} --check-input -i ${FILE} 2>&1 >/dev/null | \
        grep -E "(\*\*\* ERROR \*\*\*|Syntax OK)"`

    cd - > /dev/null

    if [[ "${STATUS}" == "Syntax OK" ]] ; then

        GOOD_SYNTAX_FILES="${GOOD_SYNTAX_FILES} ${FILE}"

    fi

done

##### CHECK PARSE AND ROUND-TRIP OF ALL INPUTS STILL IN THE GAME ######

echo
echo "Checking parse and roundtrip of all input files..."
echo

PARSE_ERROR_COUNT=0
PARSE_TEST_FILES=${GOOD_SYNTAX_FILES}

if [[ `echo ${PARSE_TEST_FILES} | wc -w` -eq 0 ]] ; then
    echo
    echo "There are no inputs to test..."
    echo
    exit 1
fi

for FILE in ${PARSE_TEST_FILES} ; do

    ##### This statement checks the parse / roundtrip of each input by doing the following:
    ##### ${INPUT_ROUNDTRIP} "${FILE}" /      :: parses the file and runs the select statement on
    #####                                        the root of the parse-tree to spit back out the
    #####                                        entire contents that were parsed
    ##### 2> ${TEMP_PARSE_FILE}               :: if there were any parse errors from the above
    #####                                     :: statement, this captures them in a temporary file
    ##### tail -n +4                          :: removes first three lines from the select, i.e.
    #####                                        (1) Selecting /
    #####                                        (2) ---- 1 nodes selected with statement '/' ----
    #####                                        (3) 1) /
    ##### diff -Bb <(sed '1s...' "${FILE}") - :: diffs roundtrip output with the actual input file
    #####                                        -B :: ignore changes where entire lines are blank
    #####                                        -b :: ignore changes in number of trailing spaces
    #####                                        sed '1s/^[[:space:]]*//' :: strip file lead space
    ##### > ${TEMP_ROUND_FILE}                :: if there were any differences from the above
    #####                                     :: statement, this captures them in a temporary file
    ##### PARSEERROR=`cat ${TEMP_PARSE_FILE}` :: capture the most recent parse error into a variable
    ##### cut -d':' -f2- | cut -d'.' -f-1`    :: capture the line number from the cached parse error
    ##### SNIPPET=`sed -n "$((${LINE}-${SN... :: capture the parse error causing text snippet and a
    #####                                        surrounding range for context from the input file

    ${INPUT_ROUNDTRIP} "${FILE}" / 2> ${TEMP_PARSE_FILE} | tail -n +4 | diff -Bb <(sed '1s/^[[:space:]]*//' "${FILE}") - > ${TEMP_ROUND_FILE}

    if [[ -s ${TEMP_PARSE_FILE} ]] ; then

        PARSE_ERROR_COUNT=$((${PARSE_ERROR_COUNT}+1))

        PARSEERROR=`cat ${TEMP_PARSE_FILE}`

        LINE=`echo ${PARSEERROR} | cut -d':' -f2- | cut -d'.' -f-1`

        SNIPPETRANGE=1

        SNIPPET=`sed -n "$((${LINE}-${SNIPPETRANGE})),$((${LINE}+${SNIPPETRANGE}))p" "${FILE}"`

        echo "---------------------------------------------------------------------------------------------"
        echo
        echo "COUNT: ${PARSE_ERROR_COUNT}"
        echo
        echo "FNAME: ${FILE}"
        echo
        echo "ERROR: ${PARSEERROR}"
        echo
        echo "INPUT:"
        echo
        echo
        echo "${SNIPPET}"
        echo
        echo "---------------------------------------------------------------------------------------------"

   elif [[ -s ${TEMP_ROUND_FILE} ]] ; then

       PARSE_ERROR_COUNT=$((${PARSE_ERROR_COUNT}+1))

       echo "${PARSE_ERROR_COUNT}\t ${FILE} ..."

       echo "ROUND TRIP DIFFERENCE:"

       cat ${TEMP_ROUND_FILE}

    else

        PARSE_PASS_LIST="${PARSE_PASS_LIST} ${FILE}"

    fi

done

##### CHECK SCHEMA VALIDATION OF ALL INPUTS STILL IN THE GAME #########

echo
echo "Checking validation of all input files against the schema..."
echo

VALID_ERROR_COUNT=0

if [[ ! -z ${PARSE_PASS_LIST} ]] ; then

    ${INPUT_VALIDATOR} ${TEMP_SCHEM_FILE} ${PARSE_PASS_LIST} | tee ${TEMP_VALID_FILE}

    VALIDATE_STATUS=$?

    echo
    echo "************************************************************************************************************"
    grep " - PASS" ${TEMP_VALID_FILE}
    echo
    echo "************************************************************************************************************"
    cat ${TEMP_VALID_FILE} | grep "line.*column" | cut -d'-' -f2- | cut -d':' -f2- | sort | uniq -c | sort -n
    echo
    echo "************************************************************************************************************"

    VALID_ERROR_COUNT=`cat ${TEMP_VALID_FILE} | grep -c " - FAIL"`

fi

##### REPORT FINAL PARSE / ROUND-TRIP / VALIDATION FAILURE RESULTS ####

echo
echo ${PARSE_ERROR_COUNT} out of `echo ${PARSE_TEST_FILES} | wc -w` FAILING TO PARSE OR ROUNDTRIP
echo ${VALID_ERROR_COUNT} out of `echo ${PARSE_PASS_LIST}  | wc -w` FAILING TO VALIDATE
echo

##### CLEANUP TEMPORARY FILES #########################################

rm -rf ${TEMP_SCHEM_FILE} 2> /dev/null
rm -rf ${TEMP_SYNTAX_DIR} 2> /dev/null
rm -rf ${TEMP_PARSE_FILE} 2> /dev/null
rm -rf ${TEMP_ROUND_FILE} 2> /dev/null
rm -rf ${TEMP_VALID_FILE} 2> /dev/null

##### RETURN ZERO IF AND ONLY IF NO PARSE OR VALIDATION ERRORS ########

if [[ ${PARSE_ERROR_COUNT} -ne 0 ]] || [[ ${VALID_ERROR_COUNT} -ne 0 ]] || [[ ${VALIDATE_STATUS} -ne 0 ]] ; then
    exit 1
else
    exit 0
fi

#######################################################################
