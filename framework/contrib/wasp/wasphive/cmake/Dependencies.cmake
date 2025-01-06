##---------------------------------------------------------------------------##
## wasphive/cmake/Dependencies.cmake
## Robert A. Lefebvre
##---------------------------------------------------------------------------##

TRIBITS_PACKAGE_DEFINE_DEPENDENCIES(
LIB_REQUIRED_PACKAGES waspcore waspsiren
LIB_OPTIONAL_PACKAGES
TEST_REQUIRED_PACKAGES waspson waspddi testframework
TEST_OPTIONAL_PACKAGES
LIB_REQUIRED_TPLS
LIB_OPTIONAL_TPLS
TEST_REQUIRED_TPLS
TEST_OPTIONAL_TPLS
)

##---------------------------------------------------------------------------##
##                     end of cmake/Dependencies.cmake
##---------------------------------------------------------------------------##

