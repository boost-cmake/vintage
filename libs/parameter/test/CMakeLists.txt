#
# Copyright Troy D. Straszheim
#
# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt
#
boost_additional_test_dependencies(optional BOOST_DEPENDS test timer)


boost_test_run(basics)
boost_test_run(sfinae)
boost_test_run(macros)
boost_test_run(earwicker)
boost_test_run(tutorial)
boost_test_run(singular)
boost_test_run(mpl)
boost_test_run(preprocessor)
boost_test_run(preprocessor_deduced)
boost_test_run(efficiency COMPILE_FLAGS "${RELEASE_COMPILE_FLAGS}")
boost_test_run(maybe)
boost_test_run(deduced)
boost_test_run(optional_deduced_sfinae)
boost_test_run(deduced_dependent_predicate)
boost_test_run(normalized_argument_types)
boost_test_compile(ntp)
boost_test_compile(unwrap_cv_reference)
boost_test_compile_fail(duplicates)
boost_test_compile_fail(deduced_unmatched_arg)
boost_test_compile(compose)

# TODO: [ bpl-test python_test ]
