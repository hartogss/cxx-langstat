import lit.formats

config.name = 'cxx-langstat'
config.test_format = lit.formats.ShTest(True)

config.suffixes = ['.c', '.cpp', '.cc']

config.test_source_root = os.path.dirname(__file__)
config.test_exec_root = os.path.join(config.my_obj_root, 'test')

config.substitutions.append(('%cxx-langstat',
    os.path.join(config.my_obj_root, 'cxx-langstat')))
