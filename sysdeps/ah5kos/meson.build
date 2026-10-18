sysdep_supported_options = {
	'posix': true,
	'linux': true,
	'glibc': true,
	'bsd': true,
}

rtld_sources += files(
	'generic/ah5kos_mlibc.cpp',
	'generic/ah5kos_spec.cpp',
	'generic/x86_64-thread.s',
)
rtld_include_dirs += include_directories('include')

libc_sources += files(
	'generic/ah5kos_mlibc.cpp',
	'generic/entry.cpp',
	'generic/ah5kos_spec.cpp',
	'generic/x86_64-thread.s',
)
libc_include_dirs += include_directories('include')

custom_target('crt1.o',
	build_by_default: true,
	input: 'crt-x86_64/crt1.S',
	output: 'crt1.o',
	command: [c_compiler.cmd_array(), '-c', '@INPUT@', '-o', '@OUTPUT@'],
	install: true,
	install_dir: 'lib'
)

custom_target('crti.o',
	build_by_default: true,
	input: 'crt-x86_64/crti.S',
	output: 'crti.o',
	command: [c_compiler.cmd_array(), '-c', '@INPUT@', '-o', '@OUTPUT@'],
	install: true,
	install_dir: 'lib'
)

custom_target('crtn.o',
	build_by_default: true,
	input: 'crt-x86_64/crtn.S',
	output: 'crtn.o',
	command: [c_compiler.cmd_array(), '-c', '@INPUT@', '-o', '@OUTPUT@'],
	install: true,
	install_dir: 'lib'
)

custom_target('Scrt1.o',
	build_by_default: true,
	input: 'crt-x86_64/Scrt1.S',
	output: 'Scrt1.o',
	command: [c_compiler.cmd_array(), '-c', '@INPUT@', '-o', '@OUTPUT@'],
	install: true,
	install_dir: 'lib'
)

if not no_headers
	install_subdir('include/abi-bits',
		install_dir: 'include',
		strip_directory: false,
		follow_symlinks: true
	)
	install_headers(
		'include/asm/ioctl.h',
		'include/asm/ioctls.h',
		subdir: 'asm',
	)
	install_headers(
		'include/ah5kos.h',
	)
endif
