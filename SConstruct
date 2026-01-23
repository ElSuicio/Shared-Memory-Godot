import os
import sys

lib_name = "shared_memory"
project_dir = "test"

env = SConscript("godot-cpp/SConstruct")
env.Append(CPPPATH=["src/"])

sources = Glob("src/*.cpp")

suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

lib_filename = "{}{}{}{}".format(env.subst('$SHLIBPREFIX'), lib_name, suffix, env.subst('$SHLIBSUFFIX'))

library = env.SharedLibrary(
    "bin/{}/{}".format(env['platform'], lib_filename),
    source = sources,
)

copy = env.Install("{}/bin/{}/".format(project_dir, env["platform"]), library)

default_args = [library, copy]
Default(*default_args)
