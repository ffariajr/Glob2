def establish_options(env):
    opts = Options('options_cache.py')
    opts.Add("CXXFLAGS", "Manually add to the CXXFLAGS", "-g")
    opts.Add("LINKFLAGS", "Manually add to the LINKFLAGS", "-g")
    opts.Add("INSTALLDIR", "Installation Directory", "/usr/local/share")
    opts.Add("BINDIR", "Binary Installation Directory", "/usr/local/bin")
    opts.Add(BoolOption("release", "Build for release", 0))
    opts.Add(BoolOption("profile", 'Build with profiling on', 0))
    opts.Add(BoolOption("mingw", "Build with mingw enabled if not auto-detected", 0))
    opts.Add(BoolOption("osx", "Build for OSX", 0))
    Help(opts.GenerateHelpText(env))
    opts.Update(env)
    opts.Save('options_cache.py', env)
    
    
class Configuration:
    """Handles the config.h file"""
    def __init__(self):
        self.f = open("config.h", 'w')
        self.f.write("// config.h. Generated by scons\n")
        self.f.write("\n")
    def add(self, variable, doc, value=""):
        self.f.write("// %s\n" % doc)
        self.f.write("#define %s %s\n" % (variable, value))
        self.f.write("\n")
    
def configure(env):
    """Configures glob2"""
    conf = Configure(env.Clone())
    configfile = Configuration()
    configfile.add("PACKAGE", "Name of package", "\"glob2\"")
    configfile.add("PACKAGE_BUGREPORT", "Define to the address where bug reports for this package should be sent.", "\"glob2-devel@nongnu.org\"")
    configfile.add("PACKAGE_DATA_DIR", "data directory", "\"" + env["INSTALLDIR"] + "\"")
    configfile.add("PACKAGE_SOURCE_DIR", "source directory", "\"" +env.Dir("#").abspath.replace("\\", "\\\\") + "\"")
    configfile.add("PACKAGE_NAME", "Define to the full name of this package.", "\"Globulation 2\"")
    configfile.add("PACKAGE_TARNAME", "Define to the one symbol short name of this package.", "\"glob2\"")
    configfile.add("PACKAGE_VERSION", "Define to the version of this package.", "\""+env["VERSION"]+"\"")
    configfile.add("AUDIO_RECORDER_OSS", "Set the audio input type to OSS; the UNIX Open Sound System")
    env.Append(CPPDEFINES=["HAVE_CONFIG_H"])
    #Simple checks for required libraries
    if not conf.CheckLib('SDL'):
        print "Could not find libSDL"
        Exit(1)
    if not conf.CheckLib('SDL_ttf'):
        print "Could not find libSDL_ttf"
        Exit(1)
    if not conf.CheckLib('SDL_image'):
        print "Could not find libSDL_image"
        Exit(1)
    if not conf.CheckLib('SDL_net'):
        print "Could not find libSDL_net"
        Exit(1)
    if not conf.CheckLib('speex') or not conf.CheckCHeader('speex/speex.h'):
        print "Could not find libspeex or could not find 'speex/speex.h'"
        Exit(1)
    if not conf.CheckLib('vorbisfile'):
        print "Could not find libvorbisfile to link against"
        Exit(1)
    if not conf.CheckCHeader('zlib.h'):
        print "Could not find zlib.h"
        Exit(1)
    else:
        if conf.CheckLib('z'):
            env.Append(LIBS="z")
        elif conf.CheckLib('zlib1'):
            env.Append(LIBS="zlib1")
        else:
            print "Coulf not find libz or zlib1.dll"
            Exit(1)

    boost_thread = ''
    if conf.CheckLib('boost_thread') and conf.CheckCXXHeader('boost/thread/thread.hpp'):
        boost_thread='boost_thread'
    elif conf.CheckLib('boost_thread-mt') and conf.CheckCXXHeader('boost/thread/thread.hpp'):
        boost_thread='boost_thread-mt'
    else:
        print "Could not find libboost_thread or libboost_thread-mt or boost/thread/thread.hpp"
        Exit(1)
    env.Append(LIBS=[boost_thread])
    

    if not conf.CheckCXXHeader('boost/shared_ptr.hpp'):
        print "Could not find boost/shared_ptr.hpp"
        Exit(1)
    if not conf.CheckCXXHeader('boost/tuple/tuple.hpp'):
        print "Could not find boost/tuple/tuple.hpp"
        Exit(1)
    if not conf.CheckCXXHeader('boost/tuple/tuple_comparison.hpp'):
        print "Could not find boost/tuple/tuple_comparison.hpp"
        Exit(1)
    if not conf.CheckCXXHeader('boost/logic/tribool.hpp'):
        print "Could not find boost/logic/tribool.hpp"
        Exit(1)
    if not conf.CheckCXXHeader('boost/lexical_cast.hpp'):
        print "Could not find boost/lexical_cast.hpp"
        Exit(1)
     
    #Do checks for OpenGL, which is different on every system
    gl_libraries = []
    if conf.CheckLib('GL') and conf.CheckCHeader('GL/gl.h'):
        gl_libraries.append("GL")
    elif conf.CheckLib('GL') and conf.CheckCHeader('OpenGL/gl.h'):
        gl_libraries.append("GL")
    elif conf.CheckLib('opengl32') and conf.CheckCHeader('GL/gl.h'):
        gl_libraries.append("opengl32")
    elif env['osx']:
    	print "Using Apple's OpenGL framework system"
    else:
        #Quick fix for OSX, ignore libraries not found
        if not env['osx']:
            print "Could not find libGL or opengl32, or could not find GL/gl.h or OpenGL/gl.h"
            Exit(1)
    
    #Do checks for GLU, which is different on every system
    if conf.CheckLib('GLU') and conf.CheckCHeader("GL/glu.h"):
        gl_libraries.append("GLU")
    elif conf.CheckLib('GLU') and conf.CheckCHeader("OpenGL/glu.h"):
        gl_libraries.append("GLU")
    elif conf.CheckLib('glu32') and conf.CheckCHeader('GL/glu.h'):
        gl_libraries.append("glu32")
    elif env['osx']:
    	print "Using Apple's OpenGL framework system"
    else:
        print "Could not find libGLU or glu32, or could not find GL/glu.h or OpenGL/glu.h"
        Exit(1)
    
    if gl_libraries:
        configfile.add("HAVE_OPENGL ", "Defined when OpenGL support is present and compiled")
        env.Append(LIBS=gl_libraries)
    
    #Do checks for fribidi
    if conf.CheckLib('fribidi') and conf.CheckCHeader('fribidi/fribidi.h'):
        configfile.add("HAVE_FRIBIDI ", "Defined when FRIBIDI support is present and compiled")
        env.Append(LIBS=['fribidi'])
    conf.Finish() 
    
    
    
def main():
    env = Environment()
    try:
        env.Clone()
    except AttributeError:
        env.Clone = env.Copy
    env["VERSION"] = "0.9.1"
    establish_options(env)
    #Add the paths to important mingw libraries
    if env['mingw'] or env['PLATFORM'] == 'win32':
        env.Append(LIBPATH=["C:/msys/1.0/local/lib", "C:/msys/1.0/lib"])
        env.Append(CPPPATH=["C:/msys/1.0/local/include/SDL", "C:/msys/1.0/local/include", "C:/msys/1.0/include/SDL", "C:/msys/1.0/include"])
    configure(env)
    env.Append(CPPPATH=['#libgag/include', '#'])
    if env['release']:
        env.Append(CXXFLAGS=' -O3')
    if env['profile']:
        env.Append(CXXFLAGS=' -pg')
        env.Append(LINKFLAGS='-pg')
        env.Append(CXXFLAGS=' -O3')
    if env['mingw'] or env['PLATFORM'] == 'win32':
        #These four options must be present before the object files when compiling in mingw
        env.Append(LINKFLAGS="-lmingw32 -lSDLmain -lSDL -mwindows")
        env.Append(LIBS=['wsock32'])
        env.ParseConfig("sh sdl-config --cflags")
        env.ParseConfig("sh sdl-config --libs")
    else:
        env.ParseConfig("sdl-config --cflags")
        env.ParseConfig("sdl-config --libs")
    if env['osx']:
        env.Append(CXXFLAGS=" -framework OpenGL ")
    env.Append(LIBS=['vorbisfile', 'SDL_ttf', 'SDL_image', 'SDL_net', 'speex'])
    
    env["TARFILE"] = env.Dir("#").abspath + "/glob2-" + env["VERSION"] + ".tar.gz"
    env["TARFLAGS"] = "-c -z"
    env.Alias("dist", env["TARFILE"])
    
    def PackTar(target, source):
    	if "dist" in COMMAND_LINE_TARGETS:
		    if not list(source) == source:
		        source = [source]
		        
		    for s in source:
		        if env.File(s).path.find("/") != -1:
		            new_dir = env.Dir("#").abspath + "/glob2-" + env["VERSION"] + "/"
		            f = env.Install(new_dir + env.File(s).path[:env.File(s).path.rfind("/")], s)
		            env.Tar(target, f)
		        else:
		            new_dir = env.Dir("#").abspath + "/glob2-" + env["VERSION"] + "/"
		            f = env.Install(new_dir, s)
		            env.Tar(target, f)
              
    PackTar(env["TARFILE"], Split("AUTHORS COPYING INSTALL mkdist mkinstall mkuninstall README README.hg SConstruct TODO"))
    
    Export('env')
    Export('PackTar')
    SConscript("campaigns/SConscript")
    SConscript("data/SConscript")
    SConscript("gnupg/SConscript")
    SConscript("libgag/SConscript")
    SConscript("libusl/SConscript")
    SConscript("maps/SConscript")
    SConscript("scripts/SConscript")
    SConscript("src/SConscript")
    SConscript("tools/SConscript")
    SConscript("windows/SConscript")
main()
