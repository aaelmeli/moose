##---------------------------------------------------------------------------##
## wasp/PackagesList.cmake
## Robert A. Lefebvre, lefebvrera@ornl.gov
## Monday November 7 2016
##---------------------------------------------------------------------------##
#
# See documentation in TriBITS https://tribits.org


##---------------------------------------------------------------------------##
## PACKAGES PROVIDED
##---------------------------------------------------------------------------##

# googletest is not present because it is not a TriBITS package.
TRIBITS_REPOSITORY_DEFINE_PACKAGES(
  testframework testframework PT
  wasp  . PT
)

TRIBITS_ALLOW_MISSING_EXTERNAL_PACKAGES(
   testframework
)

##---------------------------------------------------------------------------##
##                     end of wasp/PackagesList.cmake
##---------------------------------------------------------------------------##
