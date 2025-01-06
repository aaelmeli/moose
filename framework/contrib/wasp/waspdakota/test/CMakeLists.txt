INCLUDE(GoogleTest)

# configure paths for template tests
configure_file("Paths.h.in" "Paths.h" @ONLY)

ADD_GOOGLE_TEST(tstParam.cpp NP 1)
ADD_GOOGLE_TEST(tstSubstitution.cpp NP 1)
