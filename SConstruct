#SConstruct

# needed imports
import sys
import os
import SCons.Util

class argumentIndirection:
	def __init__(self,prefix):
		self.prefix = prefix
		self.ARGUMENTS = ARGUMENTS
	def get(self,name,value):
		return self.ARGUMENTS.get('%s_%s' % (self.prefix, name), self.ARGUMENTS.get(name,value))

# endianess-checker
def checkEndian():
    import struct
    array = struct.pack('cccc', '\x01', '\x02', '\x03', '\x04')
    i = struct.unpack('i', array)
    if i == struct.unpack('<i', array):
        return "little"
    elif i == struct.unpack('>i', array):
        return "big"
    return "unknown"

class DXXCommon:
	__endian = checkEndian()
	class UserSettings:
		def __init__(self,ARGUMENTS):
			self.OGLES_LIB = 'GLES_CM'

			# Paths for the Videocore libs/includes on the Raspberry Pi
			self.RPI_DEFAULT_VC_PATH='/opt/vc'

			self.debug = int(ARGUMENTS.get('debug', 0))
			self.profiler = int(ARGUMENTS.get('profiler', 0))
			self.opengl = int(ARGUMENTS.get('opengl', 1))
			self.opengles = int(ARGUMENTS.get('opengles', 0))
			self.opengles_lib = str(ARGUMENTS.get('opengles_lib', self.OGLES_LIB))
			self.asm = int(ARGUMENTS.get('asm', 0))
			self.editor = int(ARGUMENTS.get('editor', 0))
			self.sdlmixer = int(ARGUMENTS.get('sdlmixer', 1))
			self.ipv6 = int(ARGUMENTS.get('ipv6', 0))
			self.use_udp = int(ARGUMENTS.get('use_udp', 1))
			self.use_tracker = int(ARGUMENTS.get('use_tracker', 1))
			self.verbosebuild = int(ARGUMENTS.get('verbosebuild', 0))
			self.raspberrypi = int(ARGUMENTS.get('raspberrypi', 0))
			self.rpi_vc_path = str(ARGUMENTS.get('rpi_vc_path', self.RPI_DEFAULT_VC_PATH))
			# automatic setup for raspberrypi
			if (self.raspberrypi == 1):
				self.opengles=1
				self.opengles_lib='GLESv2'
	# Base class for platform-specific settings processing
	class _PlatformSettings:
		def __init__(self):
			self.ogllibs = ''
			self.osasmdef = None
			self.platform_sources = []
	# Settings to apply to mingw32 builds
	class Win32PlatformSettings(_PlatformSettings):
		def __init__(self,user_settings):
			DXXCommon._PlatformSettings.__init__(self)
			self.osdef = '_WIN32'
			self.osasmdef = 'win32'
		def adjust_environment(self,program,env):
			env.Append(CPPDEFINES = ['_WIN32', 'HAVE_STRUCT_TIMEVAL'])
	class DarwinPlatformSettings(_PlatformSettings):
		def __init__(self,user_settings):
			DXXCommon._PlatformSettings.__init__(self)
			self.osdef = '__APPLE__'
			user_settings.asm = 0
		def adjust_environment(self,program,env):
			env.Append(CPPDEFINES = ['HAVE_STRUCT_TIMESPEC', 'HAVE_STRUCT_TIMEVAL', '__unix__'])
	# Settings to apply to Linux builds
	class LinuxPlatformSettings(_PlatformSettings):
		def __init__(self,user_settings):
			DXXCommon._PlatformSettings.__init__(self)
			self.osdef = '__LINUX__'
			self.osasmdef = 'elf'
			if (user_settings.opengles == 1):
				self.ogllibs = [ user_settings.opengles_lib, 'EGL']
			else:
				self.ogllibs = ['GL', 'GLU']
		def adjust_environment(self,program,env):
			env.Append(CPPDEFINES = ['__LINUX__', 'HAVE_STRUCT_TIMESPEC', 'HAVE_STRUCT_TIMEVAL'])
			env.ParseConfig('pkg-config --cflags --libs sdl')

	def __init__(self):
		pass

	def prepare_environment(self):
		# Acquire environment object...
		self.env = Environment(ENV = os.environ, tools = ['mingw'])

		# Prettier build messages......
		if (self.user_settings.verbosebuild == 0):
			self.env["CCCOMSTR"]     = "Compiling $SOURCE ..."
			self.env["CXXCOMSTR"]    = "Compiling $SOURCE ..."
			self.env["LINKCOMSTR"]   = "Linking $TARGET ..."
			self.env["ARCOMSTR"]     = "Archiving $TARGET ..."
			self.env["RANLIBCOMSTR"] = "Indexing $TARGET ..."

		# Use -Wundef to catch when a shared source file includes a
		# shared header that misuses conditional compilation.  Use
		# -Werror=undef to make this fatal.  Both are needed, since
		# gcc 4.5 silently ignores -Werror=undef.  On gcc 4.5, misuse
		# produces a warning.  On gcc 4.7, misuse produces an error.
		self.env.Append(CCFLAGS = ['-Wall', '-Wundef', '-Werror=undef', '-funsigned-char', '-Werror=implicit-int', '-Werror=implicit-function-declaration', '-pthread'])
		self.env.Append(CFLAGS = ['-std=gnu99'])
		self.env.Append(CPPDEFINES = ['NETWORK'])
		self.env.Append(CPPPATH = ['common/include', 'common/main', '.'])
		# Get traditional compiler environment variables
		for cc in ['CC', 'CXX']:
			if os.environ.has_key(cc):
				self.env[cc] = os.environ[cc]
		for flags in ['CFLAGS', 'CXXFLAGS']:
			if os.environ.has_key(flags):
				self.env[flags] += SCons.Util.CLVar(os.environ[flags])

	def check_endian(self):
		# set endianess
		if (self.__endian == "big"):
			print "%s: BigEndian machine detected" % self.PROGRAM_NAME
			self.asm = 0
			self.env.Append(CPPDEFINES = ['WORDS_BIGENDIAN'])
		elif (self.__endian == "little"):
			print "%s: LittleEndian machine detected" % self.PROGRAM_NAME

	def check_platform(self):
		env = self.env
		# windows or *nix?
		if sys.platform == 'win32':
			print "%s: compiling on Windows" % self.PROGRAM_NAME
			platform = self.Win32PlatformSettings
		elif sys.platform == 'darwin':
			print "%s: compiling on Mac OS X" % self.PROGRAM_NAME
			platform = self.DarwinPlatformSettings
			sys.path += ['./arch/cocoa']
		else:
			print "%s: compiling on *NIX" % self.PROGRAM_NAME
			platform = self.LinuxPlatformSettings
		self.platform_settings = platform(self.user_settings)
		self.platform_settings.adjust_environment(self, env)
		self.common_sources += self.platform_settings.platform_sources

	def process_user_settings(self):
		env = self.env
		# opengl or software renderer?
		if (self.user_settings.opengl == 1) or (self.user_settings.opengles == 1):
			if (self.user_settings.opengles == 1):
				print "%s: building with OpenGL ES" % self.PROGRAM_NAME
				env.Append(CPPDEFINES = ['OGLES'])
			else:
				print "%s: building with OpenGL" % self.PROGRAM_NAME
			env.Append(CPPDEFINES = ['OGL'])

		# assembler code?
		if (self.user_settings.asm == 1) and (self.user_settings.opengl == 0):
			print "%s: including: ASSEMBLER" % self.PROGRAM_NAME
			env.Replace(AS = 'nasm')
			env.Append(ASCOM = ' -f ' + str(platform_settings.osasmdef) + ' -d' + str(platform_settings.osdef) + ' -Itexmap/ ')
			self.common_sources += asm_sources
		else:
			env.Append(CPPDEFINES = ['NO_ASM'])

		# SDL_mixer support?
		if (self.user_settings.sdlmixer == 1):
			print "%s: including SDL_mixer" % self.PROGRAM_NAME
			env.Append(CPPDEFINES = ['USE_SDLMIXER'])

		# debug?
		if (self.user_settings.debug == 1):
			print "%s: including: DEBUG" % self.PROGRAM_NAME
			env.Append(CPPFLAGS = ['-g'])
		else:
			env.Append(CPPDEFINES = ['NDEBUG', 'RELEASE'])
			env.Append(CPPFLAGS = ['-O2'])

		# profiler?
		if (self.user_settings.profiler == 1):
			env.Append(CPPFLAGS = ['-pg'])

		#editor build?
		if (self.user_settings.editor == 1):
			env.Append(CPPDEFINES = ['EDITOR'])
			self.common_sources += self.editor_sources

		# IPv6 compability?
		if (self.user_settings.ipv6 == 1):
			env.Append(CPPDEFINES = ['IPv6'])

		# UDP support?
		if (self.user_settings.use_udp == 1):
			env.Append(CPPDEFINES = ['USE_UDP'])
			# Tracker support?  (Relies on UDP)
			if( self.user_settings.use_tracker == 1 ):
				env.Append( CPPDEFINES = [ 'USE_TRACKER' ] )

class DXXArchive(DXXCommon):
	srcdir = 'common'
	target = 'dxx-common'
	common_sources = [os.path.join('common', f) for f in [
'2d/2dsline.c',
'2d/bitblt.c',
'2d/bitmap.c',
'2d/box.c',
'2d/canvas.c',
'2d/circle.c',
'2d/disc.c',
'2d/gpixel.c',
'2d/line.c',
'2d/pixel.c',
'2d/poly.c',
'2d/rect.c',
'2d/rle.c',
'2d/scalec.c',
'3d/clipper.c',
'3d/draw.c',
'3d/globvars.c',
'3d/instance.c',
'3d/matrix.c',
'3d/points.c',
'3d/rod.c',
'3d/setup.c',
'maths/fixc.c',
'maths/rand.c',
'maths/tables.c',
'maths/vecmat.c',
]
]
	editor_sources = []
	def __init__(self):
		self.PROGRAM_NAME = 'DXX-Archive'
		DXXCommon.__init__(self)
		self.user_settings = self.UserSettings(ARGUMENTS)
		self.prepare_environment()
		self.check_endian()
		self.check_platform()
		self.process_user_settings()
		self.register_library()
	def register_library(self):
		self.objects = self.env.StaticObject(source = self.common_sources)

class DXXProgram(DXXCommon):
	# version number
	VERSION_MAJOR = 0
	VERSION_MINOR = 57
	VERSION_MICRO = 3
	static_archive_construction = None
	class UserSettings(DXXCommon.UserSettings):
		def __init__(self,ARGUMENTS,target):
			DXXCommon.UserSettings.__init__(self, ARGUMENTS.ARGUMENTS)
			# installation path
			PREFIX = str(ARGUMENTS.get('prefix', '/usr/local'))
			self.BIN_DIR = PREFIX + '/bin'
			self.DATA_DIR = PREFIX + '/share/games/' + target
			# command-line parms
			self.sharepath = str(ARGUMENTS.get('sharepath', self.DATA_DIR))
	# Settings to apply to mingw32 builds
	class Win32PlatformSettings(DXXCommon.Win32PlatformSettings):
		def __init__(self,user_settings):
			DXXCommon.Win32PlatformSettings.__init__(self,user_settings)
			user_settings.sharepath = ''
			self.lflags = '-mwindows'
			self.libs = ['glu32', 'wsock32', 'ws2_32', 'winmm', 'mingw32', 'SDLmain', 'SDL']
		def adjust_environment(self,program,env):
			DXXCommon.Win32PlatformSettings.adjust_environment(self, program, env)
			env.RES('arch/win32/%s.rc' % program.target)
			env.Append(CPPPATH = [os.path.join(self.srcdir, 'arch/win32/include')])
			self.platform_sources = [os.path.join(program.srcdir, 'arch/win32/messagebox.c')]
	# Settings to apply to Apple builds
	# This appears to be unused.  The reference to sdl_only fails to
	# execute.
	class DarwinPlatformSettings(DXXCommon.DarwinPlatformSettings):
		def __init__(self,user_settings):
			DXXCommon.DarwinPlatformSettings.__init__(self)
			user_settings.sharepath = ''
			if (user_settings.sdlmixer == 1):
				print "including SDL_mixer"
				platform_settings.lflags += ' -framework SDL_mixer'
			# Ugly way of linking to frameworks, but kreator has seen uglier
			self.lflags = '-framework ApplicationServices -framework Carbon -framework Cocoa -framework SDL'
			if (sdl_only == 0):
				self.lflags += ' -framework OpenGL'
			self.libs = ['../physfs/build/Debug/libphysfs.dylib']
		def adjust_environment(self,program,env):
			DXXCommon.DarwinPlatformSettings.adjust_environment(self, program, env)
			self.platform_sources = [os.path.join(program.srcdir, f) for f in ['arch/cocoa/SDLMain.m', 'arch/carbon/messagebox.c']]
	# Settings to apply to Linux builds
	class LinuxPlatformSettings(DXXCommon.LinuxPlatformSettings):
		def __init__(self,user_settings):
			DXXCommon.LinuxPlatformSettings.__init__(self,user_settings)
			user_settings.sharepath += '/'
			self.lflags = os.environ["LDFLAGS"] if os.environ.has_key('LDFLAGS') else ''
		def adjust_environment(self,program,env):
			DXXCommon.LinuxPlatformSettings.adjust_environment(self, program, env)
			self.libs = env['LIBS']
			env.Append(CPPPATH = [os.path.join(program.srcdir, 'arch/linux/include')])

	def __init__(self):
		if DXXProgram.static_archive_construction is None:
			DXXProgram.static_archive_construction = DXXArchive()
		DXXCommon.__init__(self)
		self.user_settings = self.UserSettings(self.ARGUMENTS, self.target)
		self.prepare_environment()
		self.banner()
		self.check_endian()
		self.check_platform()
		self.process_user_settings()
		self.register_program()

	def prepare_environment(self):
		DXXCommon.prepare_environment(self)
		self.VERSION_STRING = ' v' + str(self.VERSION_MAJOR) + '.' + str(self.VERSION_MINOR) + '.' + str(self.VERSION_MICRO)
		self.env.Append(CPPDEFINES = [('PROGRAM_NAME', '\\"' + str(self.PROGRAM_NAME) + '\\"'), ('DXX_VERSION_MAJORi', str(self.VERSION_MAJOR)), ('DXX_VERSION_MINORi', str(self.VERSION_MINOR)), ('DXX_VERSION_MICROi', str(self.VERSION_MICRO))])

	def banner(self):
		print '\n===== ' + self.PROGRAM_NAME + self.VERSION_STRING + ' =====\n'

	def check_platform(self):
		DXXCommon.check_platform(self)
		env = self.env
		# windows or *nix?
		if sys.platform == 'darwin':
			VERSION = str(VERSION_MAJOR) + '.' + str(VERSION_MINOR)
			if (VERSION_MICRO):
				VERSION += '.' + str(VERSION_MICRO)
			env['VERSION_NUM'] = VERSION
			env['VERSION_NAME'] = self.PROGRAM_NAME + ' v' + VERSION
			import tool_bundle
		self.platform_settings.libs += ['physfs', 'm']

	def process_user_settings(self):
		DXXCommon.process_user_settings(self)
		env = self.env
		# opengl or software renderer?
		if (self.user_settings.opengl == 1) or (self.user_settings.opengles == 1):
			self.platform_settings.libs += self.platform_settings.ogllibs
			self.common_sources += self.arch_ogl_sources
		else:
			print "%s: building with Software Renderer" % self.PROGRAM_NAME
			self.common_sources += self.arch_sdl_sources

		# SDL_mixer support?
		if (self.user_settings.sdlmixer == 1):
			self.common_sources += self.arch_sdlmixer
			if (sys.platform != 'darwin'):
				self.platform_settings.libs += ['SDL_mixer']

		# profiler?
		if (self.user_settings.profiler == 1):
			self.platform_settings.lflags += ' -pg'

		#editor build?
		if (self.user_settings.editor == 1):
			env.Append(CPPPATH = [os.path.join(self.srcdir, 'include/editor')])

		# UDP support?
		if (self.user_settings.use_udp == 1):
			self.common_sources += self.sources_use_udp

		env.Append(CPPDEFINES = [('SHAREPATH', '\\"' + str(self.user_settings.sharepath) + '\\"')])

class D1XProgram(DXXProgram):
	PROGRAM_NAME = 'D1X-Rebirth'
	target = 'd1x-rebirth'
	srcdir = 'd1x-rebirth'
	ARGUMENTS = argumentIndirection('d1x')
	def prepare_environment(self):
		DXXProgram.prepare_environment(self)
		# Flags and stuff for all platforms...
		self.env.Append(CPPPATH = [os.path.join(self.srcdir, f) for f in ['include', 'main', 'arch/include']])
		self.env.Append(CPPDEFINES = [('DXX_BUILD_DESCENT_I', 1)])

	def __init__(self):
	# general source files
		self.common_sources = [os.path.join(self.srcdir, f) for f in [
'2d/font.c',
'2d/palette.c',
'2d/pcx.c',
'3d/interp.c',
'arch/sdl/event.c',
'arch/sdl/init.c',
'arch/sdl/joy.c',
'arch/sdl/key.c',
'arch/sdl/mouse.c',
'arch/sdl/rbaudio.c',
'arch/sdl/timer.c',
'arch/sdl/window.c',
'arch/sdl/digi.c',
'arch/sdl/digi_audio.c',
'iff/iff.c',
'main/ai.c',
'main/aipath.c',
'main/automap.c',
'main/bm.c',
'main/bmread.c',
'main/cntrlcen.c',
'main/collide.c',
'main/config.c',
'main/console.c',
'main/controls.c',
'main/credits.c',
'main/custom.c',
'main/digiobj.c',
'main/dumpmine.c',
'main/effects.c',
'main/endlevel.c',
'main/fireball.c',
'main/fuelcen.c',
'main/fvi.c',
'main/game.c',
'main/gamecntl.c',
'main/gamefont.c',
'main/gamemine.c',
'main/gamerend.c',
'main/gamesave.c',
'main/gameseg.c',
'main/gameseq.c',
'main/gauges.c',
'main/hostage.c',
'main/hud.c',
'main/inferno.c',
'main/kconfig.c',
'main/kmatrix.c',
'main/laser.c',
'main/lighting.c',
'main/menu.c',
'main/mglobal.c',
'main/mission.c',
'main/morph.c',
'main/multi.c',
'main/multibot.c',
'main/newdemo.c',
'main/newmenu.c',
'main/object.c',
'main/paging.c',
'main/physics.c',
'main/piggy.c',
'main/player.c',
'main/playsave.c',
'main/polyobj.c',
'main/powerup.c',
'main/render.c',
'main/robot.c',
'main/scores.c',
'main/slew.c',
'main/snddecom.c',
'main/songs.c',
'main/state.c',
'main/switch.c',
'main/terrain.c',
'main/texmerge.c',
'main/text.c',
'main/titles.c',
'main/vclip.c',
'main/wall.c',
'main/weapon.c',
'mem/mem.c',
'misc/args.c',
'misc/dl_list.c',
'misc/error.c',
'misc/hash.c',
'misc/hmp.c',
'misc/ignorecase.c',
'misc/physfsx.c',
'misc/strio.c',
'misc/strutil.c',
'texmap/ntmap.c',
'texmap/scanline.c'
#'tracker/client/tracker_client.c'
]
]

	# for editor
		self.editor_sources = [os.path.join(self.srcdir, f) for f in [
'editor/centers.c',
'editor/curves.c',
'editor/autosave.c',
'editor/eglobal.c',
'editor/ehostage.c',
'editor/elight.c',
'editor/eobject.c',
'editor/eswitch.c',
'editor/fixseg.c',
'editor/func.c',
'editor/group.c',
'editor/info.c',
'editor/kbuild.c',
'editor/kcurve.c',
'editor/kfuncs.c',
'editor/kgame.c',
'editor/khelp.c',
'editor/kmine.c',
'editor/ksegmove.c',
'editor/ksegsel.c',
'editor/ksegsize.c',
'editor/ktmap.c',
'editor/kview.c',
'editor/med.c',
'editor/meddraw.c',
'editor/medmisc.c',
'editor/medrobot.c',
'editor/medsel.c',
'editor/medwall.c',
'editor/mine.c',
'editor/objpage.c',
'editor/segment.c',
'editor/seguvs.c',
'editor/texpage.c',
'editor/texture.c',
'ui/button.c',
'ui/checkbox.c',
'ui/dialog.c',
'ui/file.c',
'ui/gadget.c',
'ui/icon.c',
'ui/inputbox.c',
'ui/keypad.c',
'ui/keypress.c',
'ui/keytrap.c',
'ui/listbox.c',
'ui/menu.c',
'ui/menubar.c',
'ui/message.c',
'ui/popup.c',
'ui/radio.c',
'ui/scroll.c',
'ui/ui.c',
'ui/uidraw.c',
'ui/userbox.c'
]
]
		DXXProgram.__init__(self)

	sources_use_udp = [os.path.join(srcdir, 'main/net_udp.c')]

	# SDL_mixer sound implementation
	arch_sdlmixer = [os.path.join(srcdir, f) for f in [
'arch/sdl/digi_mixer.c',
'arch/sdl/digi_mixer_music.c',
'arch/sdl/jukebox.c'
]
]

	# for opengl
	arch_ogl_sources = [os.path.join(srcdir, f) for f in [
'arch/ogl/gr.c',
'arch/ogl/ogl.c',
]
]

	# for non-ogl
	arch_sdl_sources = [os.path.join(srcdir, f) for f in [
'arch/sdl/gr.c',
'texmap/tmapflat.c'
]
]

	# assembler related
	asm_sources = [os.path.join(srcdir, f) for f in [
'texmap/tmap_ll.asm',
'texmap/tmap_flt.asm',
'texmap/tmapfade.asm',
'texmap/tmap_lin.asm',
'texmap/tmap_per.asm'
]
]

	def register_program(self):
		env = self.env
		exe_target = os.path.join(self.srcdir, self.target)
		# finally building program...
		env.Program(target=str(exe_target), source = self.common_sources + self.static_archive_construction.objects, LIBS = self.platform_settings.libs, LINKFLAGS = str(self.platform_settings.lflags))
		if (sys.platform != 'darwin'):
			env.Install(self.user_settings.BIN_DIR, str(exe_target))
			env.Alias('install', self.user_settings.BIN_DIR)
		else:
			tool_bundle.TOOL_BUNDLE(env)
			env.MakeBundle(self.PROGRAM_NAME + '.app', exe_target,
					'free.d1x-rebirth', 'd1xgl-Info.plist',
					typecode='APPL', creator='DCNT',
					icon_file='arch/cocoa/d1x-rebirth.icns',
					subst_dict={'d1xgl' : exe_target},	# This is required; manually update version for Xcode compatibility
					resources=[['English.lproj/InfoPlist.strings', 'English.lproj/InfoPlist.strings']])

class D2XProgram(DXXProgram):
	PROGRAM_NAME = 'D2X-Rebirth'
	target = 'd2x-rebirth'
	srcdir = 'd2x-rebirth'
	ARGUMENTS = argumentIndirection('d2x')
	def prepare_environment(self):
		DXXProgram.prepare_environment(self)
		# Flags and stuff for all platforms...
		self.env.Append(CPPPATH = [os.path.join(self.srcdir, f) for f in ['include', 'main', 'arch/include']])
		self.env.Append(CPPDEFINES = [('DXX_BUILD_DESCENT_II', 1)])

	def __init__(self):
	# general source files
		self.common_sources = [os.path.join(self.srcdir, f) for f in [
'2d/font.c',
'2d/palette.c',
'2d/pcx.c',
'3d/interp.c',
'arch/sdl/event.c',
'arch/sdl/init.c',
'arch/sdl/joy.c',
'arch/sdl/key.c',
'arch/sdl/mouse.c',
'arch/sdl/rbaudio.c',
'arch/sdl/timer.c',
'arch/sdl/window.c',
'arch/sdl/digi.c',
'arch/sdl/digi_audio.c',
'iff/iff.c',
'libmve/decoder8.c',
'libmve/decoder16.c',
'libmve/mve_audio.c',
'libmve/mvelib.c',
'libmve/mveplay.c',
'main/ai.c',
'main/ai2.c',
'main/aipath.c',
'main/automap.c',
'main/bm.c',
'main/cntrlcen.c',
'main/collide.c',
'main/config.c',
'main/console.c',
'main/controls.c',
'main/credits.c',
'main/digiobj.c',
'main/dumpmine.c',
'main/effects.c',
'main/endlevel.c',
'main/escort.c',
'main/fireball.c',
'main/fuelcen.c',
'main/fvi.c',
'main/game.c',
'main/gamecntl.c',
'main/gamefont.c',
'main/gamemine.c',
'main/gamepal.c',
'main/gamerend.c',
'main/gamesave.c',
'main/gameseg.c',
'main/gameseq.c',
'main/gauges.c',
'main/hostage.c',
'main/hud.c',
'main/inferno.c',
'main/kconfig.c',
'main/kmatrix.c',
'main/laser.c',
'main/lighting.c',
'main/menu.c',
'main/mglobal.c',
'main/mission.c',
'main/morph.c',
'main/movie.c',
'main/multi.c',
'main/multibot.c',
'main/newdemo.c',
'main/newmenu.c',
'main/object.c',
'main/paging.c',
'main/physics.c',
'main/piggy.c',
'main/player.c',
'main/playsave.c',
'main/polyobj.c',
'main/powerup.c',
'main/render.c',
'main/robot.c',
'main/scores.c',
'main/segment.c',
'main/slew.c',
'main/songs.c',
'main/state.c',
'main/switch.c',
'main/terrain.c',
'main/texmerge.c',
'main/text.c',
'main/titles.c',
'main/vclip.c',
'main/wall.c',
'main/weapon.c',
'mem/mem.c',
'misc/args.c',
'misc/dl_list.c',
'misc/error.c',
'misc/hash.c',
'misc/hmp.c',
'misc/ignorecase.c',
'misc/physfsrwops.c',
'misc/physfsx.c',
'misc/strio.c',
'misc/strutil.c',
'texmap/ntmap.c',
'texmap/scanline.c'
]
]

	# for editor
		self.editor_sources = [os.path.join(self.srcdir, f) for f in [
'editor/centers.c',
'editor/curves.c',
'editor/autosave.c',
'editor/eglobal.c',
'editor/ehostage.c',
'editor/elight.c',
'editor/eobject.c',
'editor/eswitch.c',
'editor/fixseg.c',
'editor/func.c',
'editor/group.c',
'editor/info.c',
'editor/kbuild.c',
'editor/kcurve.c',
'editor/kfuncs.c',
'editor/kgame.c',
'editor/khelp.c',
'editor/kmine.c',
'editor/ksegmove.c',
'editor/ksegsel.c',
'editor/ksegsize.c',
'editor/ktmap.c',
'editor/kview.c',
'editor/med.c',
'editor/meddraw.c',
'editor/medmisc.c',
'editor/medrobot.c',
'editor/medsel.c',
'editor/medwall.c',
'editor/mine.c',
'editor/objpage.c',
'editor/segment.c',
'editor/seguvs.c',
'editor/texpage.c',
'editor/texture.c',
'main/bmread.c',
'ui/button.c',
'ui/checkbox.c',
'ui/dialog.c',
'ui/file.c',
'ui/gadget.c',
'ui/icon.c',
'ui/inputbox.c',
'ui/keypad.c',
'ui/keypress.c',
'ui/keytrap.c',
'ui/listbox.c',
'ui/menu.c',
'ui/menubar.c',
'ui/message.c',
'ui/popup.c',
'ui/radio.c',
'ui/scroll.c',
'ui/ui.c',
'ui/uidraw.c',
'ui/userbox.c'
]
]
		DXXProgram.__init__(self)

	sources_use_udp = [os.path.join(srcdir, 'main/net_udp.c')]

	# SDL_mixer sound implementation
	arch_sdlmixer = [os.path.join(srcdir, f) for f in [
'arch/sdl/digi_mixer.c',
'arch/sdl/digi_mixer_music.c',
'arch/sdl/jukebox.c'
]
]

	# for opengl
	arch_ogl_sources = [os.path.join(srcdir, f) for f in [
'arch/ogl/gr.c',
'arch/ogl/ogl.c',
]
]

	# for non-ogl
	arch_sdl_sources = [os.path.join(srcdir, f) for f in [
'arch/sdl/gr.c',
'texmap/tmapflat.c'
]
]

	# assembler related
	asm_sources = [os.path.join(srcdir, f) for f in [
'texmap/tmap_ll.asm',
'texmap/tmap_flt.asm',
'texmap/tmapfade.asm',
'texmap/tmap_lin.asm',
'texmap/tmap_per.asm'
]
]

	def register_program(self):
		env = self.env
		exe_target = os.path.join(self.srcdir, self.target)
		# finally building program...
		env.Program(target=str(exe_target), source = self.common_sources + self.static_archive_construction.objects, LIBS = self.platform_settings.libs, LINKFLAGS = str(self.platform_settings.lflags))
		if (sys.platform != 'darwin'):
			env.Install(self.user_settings.BIN_DIR, str(exe_target))
			env.Alias('install', self.user_settings.BIN_DIR)
		else:
			tool_bundle.TOOL_BUNDLE(env)
			env.MakeBundle(self.PROGRAM_NAME + '.app', exe_target,
					'free.d2x-rebirth', 'd2xgl-Info.plist',
					typecode='APPL', creator='DCNT',
					icon_file='arch/cocoa/d2x-rebirth.icns',
					subst_dict={'d2xgl' : exe_target},	# This is required; manually update version for Xcode compatibility
					resources=[['English.lproj/InfoPlist.strings', 'English.lproj/InfoPlist.strings']])

program_d1x = None
program_d2x = None
if int(ARGUMENTS.get('d1x', 1)):
	program_d1x = D1XProgram()
if int(ARGUMENTS.get('d2x', 1)):
	program_d2x = D2XProgram()

# show some help when running scons -h
Help('DXX-Rebirth, SConstruct file help:' +
	"""

	Type 'scons' to build the binary.
	Type 'scons install' to build (if it hasn't been done) and install.
	Type 'scons -c' to clean up.
	
	Extra options (add them to command line, like 'scons extraoption=value'):
	
	'sharepath=[DIR]'     (non-Mac OS *NIX only) use [DIR] for shared game data. [default: /usr/local/share/games/d2x-rebirth]
	'opengl=[0/1]'        build with OpenGL support [default: 1]
	'opengles=[0/1]'      build with OpenGL ES support [default: 0]
	'opengles_lib=[NAME]' specify the name of the OpenGL ES library to link against
	'sdlmixer=[0/1]'      build with SDL_Mixer support for sound and music (includes external music support) [default: 1]
	'asm=[0/1]'           build with ASSEMBLER code (only with opengl=0, requires NASM and x86) [default: 0]
	'debug=[0/1]'         build DEBUG binary which includes asserts, debugging output, cheats and more output [default: 0]
	'profiler=[0/1]'      profiler build [default: 0]
	'editor=[0/1]'        include editor into build (!EXPERIMENTAL!) [default: 0]
	'ipv6=[0/1]'          enable IPv6 compability [default: 0]
	'use_udp=[0/1]'       enable UDP support [default: 1]
	'use_tracker=[0/1]'   enable Tracker support (requires udp) [default :1]
	'verbosebuild=[0/1]'  print out all compiler/linker messages during building [default: 0]
	'raspberrypi=[0/1]'   build for Raspberry Pi (automatically sets opengles and opengles_lib) [default: 0]
	'rpi_vc_path=[DIR]'   use [DIR] to look for VideoCore libraries/header files (RPi only)

	Default values:
""" +
	(('	 d1x sharepath = ' + program_d1x.user_settings.DATA_DIR + '\n') if program_d1x else '') +
	(('	 d2x sharepath = ' + program_d2x.user_settings.DATA_DIR + '\n') if program_d2x else '') +
	(('	 d2x opengles_lib = ' + program_d2x.user_settings.OGLES_LIB + '\n') if program_d2x else '') +
	(('	 d2x rpi_vc_path = ' + program_d2x.user_settings.RPI_DEFAULT_VC_PATH + '\n') if program_d2x else '') +
"""
	Some influential environment variables:
	  CC          C compiler command
	  CFLAGS      C compiler flags
	  LDFLAGS     linker flags, e.g. -L<lib dir> if you have libraries in a
                      nonstandard directory <lib dir>
                      <include dir>
	  CXX         C++ compiler command
	  CXXFLAGS    C++ compiler flags
        """)

#EOF
