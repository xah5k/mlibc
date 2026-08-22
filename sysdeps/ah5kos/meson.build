sysdep_supported_options = {
	'posix': true,
	'linux': true,
	'glibc': true,
	'bsd': true,
}
libc_sources += files(
	'generic/ah5kos.cpp',
    'generic/entry.cpp',
)
custom_target('crt1.o',
	build_by_default: true,
	input: 'crt-x86_64/crt1.S',
	output: 'crt1.o',
	command: [c_compiler.cmd_array(), '-c', '@INPUT@', '-o', '@OUTPUT@'],
	install: true,
	install_dir: 'lib'
)

if not no_headers
	# Dynamically copy the whole folder directly to your sysroot destination
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
endif
