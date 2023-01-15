from conans import ConanFile


class cpp_templateConan(ConanFile):
    """
    Dependencies for cpp_template app.
    """
    name = "cpp_template"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    options = {
        "testing": [True, False]
    }
    default_options = {
        "testing": False
    }

    def requirements(self):
        if self.options.testing:
            self.requires('gtest/1.11.0')
