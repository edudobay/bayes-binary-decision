def createStaticPicLibraryBuilder(env):
   """This is a utility function that creates the StaticExtLibrary Builder in
   an Environment if it is not there already.

   If it is already there, we return the existing one."""
   import SCons.Action

   try:
      static_extlib = env['BUILDERS']['StaticPicLibrary']
   except KeyError:
      action_list = [ SCons.Action.Action("$ARCOM", "$ARCOMSTR") ]
      if env.Detect('ranlib'):
         ranlib_action = SCons.Action.Action("$RANLIBCOM", "$RANLIBCOMSTR")
         action_list.append(ranlib_action)

   static_extlib = SCons.Builder.Builder(action = action_list,
                                         emitter = '$LIBEMITTER',
                                         prefix = '$LIBPREFIX',
                                         suffix = '$LIBSUFFIX',
                                         src_suffix = '$OBJSUFFIX',
                                         src_builder = 'SharedObject')

   env['BUILDERS']['StaticPicLibrary'] = static_extlib
   return static_extlib

env = Environment(
   CXXFLAGS=['-std=gnu++11', '-Wall'],
   LIBS=['-lm']
)

createStaticPicLibraryBuilder(env)

libsources = Split("""
   agent.cc
   markov.cc
   rand.cc
   runner.cc
""")

shared = env.Clone()
shared.VariantDir('build/shared', 'core', duplicate=0)
shared_lib = shared.SharedLibrary('build/shared/decider', File(libsources, 'build/shared'))
staticdyn_lib = shared.StaticPicLibrary('build/shared/decider_dyn', File(libsources, 'build/shared'))

static = env.Clone()
static.VariantDir('build/static', 'core', duplicate=0)
static_lib = static.StaticLibrary('build/static/decider', File(libsources, 'build/static'))

env.Default([shared_lib, staticdyn_lib, static_lib])

env.VariantDir('build', 'core', duplicate=0)
test = env.Program('build/test', File(['main.cc'] + libsources), 'build')
env.Alias('test', test)

# vim:set ft=python sw=3:
