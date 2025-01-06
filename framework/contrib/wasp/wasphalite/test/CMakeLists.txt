INCLUDE(GoogleTest)

# configure paths for template tests
configure_file("Paths.h.in" "Paths.h" @ONLY)

# single template expected error
configure_file("data/file_import_file_missing.result.txt.in" "file_import_file_missing.result.txt" @ONLY)
configure_file("data/unmatched_conditional_terminator.result.txt.in" "unmatched_conditional_terminator.result.txt" @ONLY)
configure_file("data/unmatched_conditional_elseif.result.txt.in" "unmatched_conditional_elseif.result.txt" @ONLY)
configure_file("data/unmatched_conditional_else.result.txt.in" "unmatched_conditional_else.result.txt" @ONLY)
configure_file("data/undefined_attribute.result.txt.in" "undefined_attribute.result.txt" @ONLY)
configure_file("data/expression_evaluation_syntax_error.result.txt.in" "expression_evaluation_syntax_error.result.txt" @ONLY)
configure_file("data/expression_evaluation_format_error.result.txt.in" "expression_evaluation_format_error.result.txt" @ONLY)
configure_file("data/iterative_expression_range_error.result.txt.in" "iterative_expression_range_error.result.txt" @ONLY)
configure_file("data/scoped_expression_evaluation_error.result.txt.in" "scoped_expression_evaluation_error.result.txt" @ONLY)
configure_file("data/scoped_iterative_expression_evaluation_error.result.txt.in" "scoped_iterative_expression_evaluation_error.result.txt" @ONLY)
configure_file("data/file_repeat_error.result.txt.in" "file_repeat_error.result.txt" @ONLY)
configure_file("data/attribute_options_missing_delimiter_error.result.txt.in" "attribute_options_missing_delimiter_error.result.txt" @ONLY)
configure_file("data/empty_attribute_substitution_error.result.txt.in" "empty_attribute_substitution_error.result.txt" @ONLY)

# Same errors but with imports to demonstrate/test stack trace

configure_file("data/import_with_file_import_file_missing.result.txt.in" "import_with_file_import_file_missing.result.txt" @ONLY)
configure_file("data/import_with_unmatched_conditional_terminator.result.txt.in" "import_with_unmatched_conditional_terminator.result.txt" @ONLY)
configure_file("data/import_with_unmatched_conditional_elseif.result.txt.in" "import_with_unmatched_conditional_elseif.result.txt" @ONLY)
configure_file("data/import_with_unmatched_conditional_else.result.txt.in" "import_with_unmatched_conditional_else.result.txt" @ONLY)
configure_file("data/import_with_undefined_attribute.result.txt.in" "import_with_undefined_attribute.result.txt" @ONLY)
configure_file("data/import_with_expression_evaluation_syntax_error.result.txt.in" "import_with_expression_evaluation_syntax_error.result.txt" @ONLY)
configure_file("data/import_with_expression_evaluation_format_error.result.txt.in" "import_with_expression_evaluation_format_error.result.txt" @ONLY)
configure_file("data/import_with_iterative_expression_range_error.result.txt.in" "import_with_iterative_expression_range_error.result.txt" @ONLY)
configure_file("data/import_with_scoped_expression_evaluation_error.result.txt.in" "import_with_scoped_expression_evaluation_error.result.txt" @ONLY)
configure_file("data/import_with_scoped_iterative_expression_evaluation_error.result.txt.in" "import_with_scoped_iterative_expression_evaluation_error.result.txt" @ONLY)
configure_file("data/import_with_file_repeat_error.result.txt.in" "import_with_file_repeat_error.result.txt" @ONLY)
configure_file("data/import_with_attribute_options_missing_delimiter_error.result.txt.in" "import_with_attribute_options_missing_delimiter_error.result.txt" @ONLY)
configure_file("data/import_with_empty_attribute_substitution_error.result.txt.in" "import_with_empty_attribute_substitution_error.result.txt" @ONLY)


ADD_GOOGLE_TEST(tstHaliteTreeConstruction.cpp NP 1)
ADD_GOOGLE_TEST(tstHaliteEngine.cpp NP 1)
ADD_GOOGLE_TEST(tstHaliteErrors.cpp NP 1)
ADD_GOOGLE_TEST(tstSubStringIndexer.cpp NP 1)
ADD_GOOGLE_TEST(tstDataAccessor.cpp NP 1)
ADD_GOOGLE_TEST(tstHaliteWorkflow.cpp NP 1)
ADD_GOOGLE_TEST(tstHaliteNodeView.cpp NP 1)

