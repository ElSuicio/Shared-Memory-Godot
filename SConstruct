import os
import sys

lib_name = "shared_memory"
project_dir = "test-shared-memory"

env = SConscript("third_party/godot-cpp/SConstruct")
env.Append(CPPPATH=["src/"])

common_sources = Glob("src/*.cpp")
platform_sources = []

if env["platform"] == "windows":
    platform_sources += Glob("src/*_windows.cpp")
elif env["platform"] == "linux":
    platform_sources += Glob("src/*_posix.cpp")

sources = []

for source in common_sources:
    filename = str(source)

    if filename.endswith("_windows.cpp"):
        continue
    
    if filename.endswith("_posix.cpp"):
        continue
        
    sources.append(source)


sources += platform_sources

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source = Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

lib_filename = "{}{}{}{}".format(env.subst('$SHLIBPREFIX'), lib_name, suffix, env.subst('$SHLIBSUFFIX'))

library = env.SharedLibrary(
    "bin/{}/{}".format(env['platform'], lib_filename),
    source = sources,
)

copy = env.Install("{}/bin/{}/".format(project_dir, env["platform"]), library)

default_args = [library, copy]
Default(*default_args)
