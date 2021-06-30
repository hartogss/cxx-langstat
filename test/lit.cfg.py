# Configuration file for test suite

import lit.formats

# Test suite name
config.name = 'cxx-langstat'

#
config.test_format = lit.formats.ShTest(True)

# Only treat files with these suffixes as test file
config.suffixes = ['.c', '.cpp', '.cc']

#
config.test_source_root = os.path.dirname(__file__)

# Where tests are run
config.test_exec_root = os.path.join(config.cxxlangstat_obj_root, 'test')

# In the source files of tests that are part of this suite, we can use
# the following 'macros'
config.substitutions.append(('%cxx-langstat',
    os.path.join(config.cxxlangstat_obj_root, 'cxx-langstat')))

config.substitutions.append(('%clangxx', config.clangxx))
