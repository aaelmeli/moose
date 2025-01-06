##---------------------------------------------------------------------------##
## wasputils/cmake/Dependencies.cmake
## Robert A. Lefebvre
##---------------------------------------------------------------------------##

TRIBITS_PACKAGE_DEFINE_DEPENDENCIES(
LIB_REQUIRED_PACKAGES waspcore wasphive waspsiren
LIB_OPTIONAL_PACKAGES wasphit waspson waspddi wasphalite waspjson waspeddi wasplsp waspdakota
TEST_REQUIRED_PACKAGES testframework
TEST_OPTIONAL_PACKAGES
LIB_REQUIRED_TPLS
LIB_OPTIONAL_TPLS
TEST_REQUIRED_TPLS
TEST_OPTIONAL_TPLS
)

##---------------------------------------------------------------------------##
##                     end of SON/cmake/Dependencies.cmake
##---------------------------------------------------------------------------##

