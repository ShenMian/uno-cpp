set_languages("c++20")

if is_mode("debug") then
    set_symbols("debug")
end

add_requires("sfml 3.0.0", "doctest 2.4.11")
add_packages("sfml", "doctest")

includes("xmake/**.lua")

target("uno")
    set_kind("binary")
    set_warnings("all", "error")
    add_files("src/**.cpp")
    remove_files("tests/**.cpp")
    set_installdir("$(projectdir)/dist")
    after_build(function (target)
        os.cp(target:targetfile(), path.join(target:installdir(), path.filename(target:targetfile())))
    end)

target("test")
    set_kind("binary")
    set_warnings("all", "error")
    add_files("tests/**.cpp")
    add_packages("doctest")