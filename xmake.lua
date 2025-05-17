set_languages("c++20")

add_requires("sfml 3.0.0", "doctest 2.4.11")
add_packages("sfml", "doctest")

target("uno")
    set_kind("binary")
    set_warnings("all", "error")
    add_files("src/**.cpp")

target("test")
    set_kind("binary")
    set_warnings("all", "error")
    add_files("src/**.cpp")
    for _, testfile in ipairs(os.files("tests/*.cpp")) do
        add_tests(path.basename(testfile), {
            files = testfile,
            remove_files = "src/main.cpp"})
    end
