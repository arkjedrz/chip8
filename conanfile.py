from conan import ConanFile


class Chip8Conan(ConanFile):
    """
    Dependencies for Chip8 app.
    """
    name = "Chip8"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    options = {
        "testing": [True, False]
    }
    default_options = {
        "testing": False
    }

    def requirements(self):
        self.requires('cli11/2.2.0')
        self.requires('sdl/2.0.20')
        if self.options.testing:
            self.requires('gtest/1.11.0')

    def configure(self):
        self.options['sdl'].wayland = False
