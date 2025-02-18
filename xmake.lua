add_requires("sfml 3.0.0")

set_languages("c++20")

target("uno")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("sfml")
