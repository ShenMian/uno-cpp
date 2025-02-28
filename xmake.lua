add_requires("sfml 3.0.0")

set_languages("c++20")

if is_mode("debug") then
    set_symbols("debug")
end

target("uno")
    set_kind("binary")
    set_warnings("all", "error")
    add_files("src/*.cpp")
    add_packages("sfml")
