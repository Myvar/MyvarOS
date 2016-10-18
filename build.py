#!/usr/bin/python3
import subprocess
import sys
import os
import argparse
import config
import shutil

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def print_risk(title, msg):
	if 'QUIET' in os.environ and os.environ['QUIET'] == '1':
		return
	print(bcolors.OKBLUE + '[' + title + '] ' + bcolors.OKBLUE + msg + bcolors.ENDC)

def print_good(title, msg):
	if 'QUIET' in os.environ and os.environ['QUIET'] == '1':
		return	
	print(bcolors.OKGREEN + '[' + title + '] ' + bcolors.OKGREEN + msg + bcolors.ENDC)	

def print_fail(title, msg):
	if 'QUIET' in os.environ and os.environ['QUIET'] == '1':
		return	
	print(bcolors.HEADER + '[' + title + ']\n' + bcolors.FAIL + msg + bcolors.ENDC)

def print_linking(msg):
	if 'QUIET' in os.environ and os.environ['QUIET'] == '1':
		return	
	print(bcolors.OKBLUE + '[LINKING] ' + bcolors.OKBLUE + msg + bcolors.ENDC)

def print_compiling(msg):
	if 'QUIET' in os.environ and os.environ['QUIET'] == '1':
		return	
	print(bcolors.OKBLUE + '[BUILDING] ' + bcolors.OKBLUE + msg + bcolors.ENDC)

def print_compile_skip(msg):
	if 'QUIET' in os.environ and os.environ['QUIET'] == '1':
		return	
	print(bcolors.OKGREEN + '[OK] ' + bcolors.OKGREEN + msg + bcolors.ENDC)

def print_compile_fail(msg):
	if 'QUIET' in os.environ and os.environ['QUIET'] == '1':
		return	
	print(bcolors.HEADER + '[COMPILE-ERRORS]\n' + bcolors.FAIL + msg + bcolors.ENDC)

def transform_opts(opts, envadds):
	out = []

	for k in envadds:
		os.environ[k] = envadds[k]

	for opt in opts:
		out.append(os.path.expandvars(opt))

	return out

def run(cmdline, stdout=None, stderr=None, comm_timeout=120):
	if 'MAKE_SHELL_SCRIPT' in os.environ and os.environ['MAKE_SHELL_SCRIPT'] == '1':
		print(cmdline)
		return (b'', b'')
	if 'VERBOSE' in os.environ and os.environ['VERBOSE'] == '1':
		print(cmdline)
	proc = subprocess.Popen(cmdline, shell=True, stderr=stderr, stdout=stdout)
	return proc.communicate(timeout=comm_timeout)

def copyfile(src, dst):
	if 'MAKE_SHELL_SCRIPT' in os.environ and os.environ['MAKE_SHELL_SCRIPT'] == '1':
		print('cp %s %s' % (src, dst))
		return
	shutil.copyfile(src, dst)

def action_csharp_build(copts, sources, output):
	sources_abs = []

	for src in sources:
		sources_abs.append(os.path.abspath(src))

	opts = transform_opts(config.SEDNA_CSHARP_COMPILER_OPTS, {
		'BASE_DIR':	    os.path.abspath('./'),
		'INPUT': 		' '.join(sources_abs),
		'OUTPUT': 		output,
	})

	return run(' '.join(opts), stderr=subprocess.PIPE)

def action_build_sedna(copts):
	if os.path.exists('./bin') is False:
		os.makedirs('./bin')

	(outs, errs) = action_csharp_build(copts, config.SEDNA_SOURCES, '${BASE_DIR}/bin/compiler.exe')

	if len(errs) > 0:
		print_compile_fail(errs.decode('utf8'))
		print('Build stopped because of errors.')
		exit(-1)

def action_build_sedna_kernel_module(copts):
	if os.path.exists('./bin') is False:
		os.makedirs('./bin')

	# Build sedna kernel module bytecode.
	opts = ['${BASE_DIR}/bin/compiler.exe', '${BASE_DIR}/bin/main.sbc', '${BASE_DIR}/sedna/main.sn']
	opts = transform_opts(opts, {
		'BASE_DIR':	    os.path.abspath('./'),
	})
	(outs, errs) = run(' '.join(opts), stderr=subprocess.PIPE)
	if len(errs) > 0:
		print_compile_fail(errs.decode('utf8'))
		print('Build stopped because of errors.')
		exit(-1)	

	fd = open('./bin/sedna_wrap.asm', 'w')
	fd.write('BITS 32\n')
	fd.write('SECTION .sedna\n')
	fd.write('INCBIN "%s/main.sbc"\n' % os.path.abspath('./bin/'))
	fd.close()

	# Build sedna kernel module.
	opts = ['${NASM_BIN}', '-g', '-f', 'elf', '-o', '${BASE_DIR}/bin/ksedna.o', '${BASE_DIR}/bin/sedna_wrap.asm']
	opts = transform_opts(opts, {
		'BASE_DIR':	    os.path.abspath('./'),
	})

	(outs, errs) = run(' '.join(opts), stderr=subprocess.PIPE)
	if len(errs) > 0:
		print_compile_fail(errs.decode('utf8'))
		print('Build stopped because of errors.')
		exit(-1)	



def action_build_kernel(copts):
	if os.path.exists('./bin') is False:
		os.makedirs('./bin')

	objs = []

	base_dir = os.path.abspath('./')

	for src in config.KERNEL_SOURCES:
		ignore_stderr = False

		if type(src) is dict:
			if 'ignore_stderr' in src and src['ignore_stderr'] is True:
				ignore_stderr = True
			src = src['src']

		fsrc = os.path.abspath(src)
		(bsrc, esrc) = os.path.splitext(fsrc)

		(_, src) = os.path.split(fsrc)

		if esrc == '.o':
			objs.append(fsrc)
			continue

		fout = os.path.abspath('bin/%s.o' % (src))

		objs.append(fout)

		# Is object file older than source?
		src_mtime = os.path.getmtime(fsrc)
		if os.path.exists(fout):
			obj_mtime = os.path.getmtime(fout)
		else:
			obj_mtime = 0

		if obj_mtime >= src_mtime:
			print_compile_skip('%s' % (src))
			continue

		print_compiling('%s' % (src))

		if esrc == '.asm':
			opts = transform_opts(config.NASM_OPTS, {
				'BASE_DIR':	    base_dir,
				'INPUT': 		fsrc,
				'OUTPUT': 		fout,
			})
		elif esrc == '.c':
			opts = transform_opts(config.GCC_OPTS, {
				'BASE_DIR':		base_dir,
				'INPUT':		fsrc,
				'OUTPUT':		fout,
			})
		else:
			print('The extension "%s" is unknown for source "%s".' % (esrc, src))
			exit(-1)

		(outs, errs) = run(' '.join(opts), stderr=subprocess.PIPE)
		
		if len(errs) > 0:
			print_compile_fail(errs.decode('utf8'))
			if ignore_stderr is False:
				print('Build stopped because of errors.')
				exit(-1)

	opts = transform_opts(config.LD_OPTS, {
		'BASE_DIR': 		base_dir,
		'INPUT':			' '.join(objs),
		'OUTPUT':			os.path.abspath('./bin/krnlld.bin'),
	})

	print_linking('krnlld.bin')

	(outs, errs) = run(' '.join(opts), stderr=subprocess.PIPE)

	if len(errs) > 0:
		print_compile_fail(errs.decode('utf8'))
		exit(-1)

	print_risk('MAKING-ISO', 'myvaros.iso')

	if os.path.exists('./bin/iso') is False:
		os.makedirs('./bin/iso')

	opts = transform_opts(config.KERNEL_ISO_OPTS, {
		'BASE_DIR': 		base_dir,
		'ISO_DIR':			os.path.abspath('./bin/iso'),
		'INPUT':			' '.join(objs),
		'OUTPUT':			os.path.abspath('./bin/myvaros.iso'),
	})

	copyfile('./isocfg/isolinux-debug.bin', './bin/iso/isolinux-debug.bin')
	copyfile('./isocfg/ldlinux.c32', './bin/iso/ldlinux.c32')
	copyfile('./isocfg/libcom32.c32', './bin/iso/libcom32.c32')
	copyfile('./isocfg/isolinux.cfg', './bin/iso/isolinux.cfg')
	copyfile('./isocfg/mboot.c32', './bin/iso/mboot.c32')
	copyfile('./bin/krnlld.bin', './bin/iso/krnlld.bin')

	if 'VERBOSE' in os.environ and os.environ['VERBOSE'] == '1':
		print(' '.join(opts))

	(outs, errs) = run(' '.join(opts), stderr=subprocess.PIPE)

	if len(errs) > 0:
		print_fail('ISO CREATION FAILED', errs.decode('utf8'))
		exit(-1)

	print_good('RESULT', 'BUILD SUCCESS')

def action_run(copts):
	if copts.system[0] != 'kernel':
		print('Only supported system to run is "kernel".')
		exit(-1)

	opts = transform_opts(config.KERNEL_QEMU_OPTS, {
		'BASE_DIR': 		os.path.abspath('./'),
		'BIN_DIR':			os.path.abspath('./bin'),
		'ISO_DIR':			os.path.abspath('./bin/iso'),
		'INPUT':	    	os.path.abspath('./bin/myvaros.iso'),
	})

	try:
		run(' '.join(opts), stdout=sys.stdout, stderr=sys.stderr, comm_timeout=None)
	except:
		pass

"""
	The system targets for build operations.
"""
systems_build = {
	'kernel': {
		'handler': action_build_kernel,
		'deps': [
			'sedna_kernel_module',
		]
	},
	'sedna_kernel_module': {
		'handler': action_build_sedna_kernel_module,
		'deps': {
			'sedna_compiler',
		}
	},
	'sedna_compiler': {
		'handler': action_build_sedna,
		'deps': [
		]
	},
}

class UnknownTargetSystem(Exception):
	def __init__(self, sys_name):
		super().__init__()
		self.sys_name = sys_name

def action_build(copts, sysname_override=None):
	if sysname_override is not None:
		sys_name = sysname_override
	else:
		sys_name = copts.system[0]
		print_risk('BUILDING', sys_name)

	if systems_build.get(sys_name, None) is None:
		raise UnknownTargetSystem(sys_name)

	cfg = systems_build[sys_name]

	if cfg.get('deps', None) is not None:
		for dep in cfg['deps']:
			print_risk('DEP-BUILDING', dep)
			action_build(copts, dep)

	if cfg.get('handler', None) is not None:
		cfg['handler'](copts)

	print_good('BUILD', 'completed build of %s' % sys_name)


def main(args):
	for bin_name in config.BIN_PATHS:
		if bin_name not in os.environ:
			os.environ[bin_name] = config.BIN_PATHS[bin_name]

	# Read configuration containing the source files.
	parser = argparse.ArgumentParser(description='Build Coordination')
	parser.add_argument('action', type=str, nargs=1, help='The action.')
	parser.add_argument('system', type=str, nargs=1, help='The system to build.')
	parser.add_argument('-no-wait', action='store_true', default=False)
	parser.add_argument('-make-shell-script', action='store_true', default=False)
	copts = parser.parse_args(args[1:])

	if copts.make_shell_script:
		os.environ['MAKE_SHELL_SCRIPT'] = '1'
		os.environ['QUIET'] = '1'

	if copts.no_wait:
		os.environ['QEMU_SERIAL_DEBUG_NOWAIT'] = ',nowait'
	else:
		os.environ['QEMU_SERIAL_DEBUG_NOWAIT'] = ''
	
	if copts.action[0] == 'build' or copts.action[0] == 'run':
		action_build(copts)
	else:
		print('Only supported actions are "build" and "run".')

	if copts.action[0] == 'run':
		action_run(copts)

try:
	main(sys.argv)
except UnknownTargetSystem as exc:
	print('The system "%s" is not known.' % exc.sys_name)
	syslst = list(systems_build.keys())
	syslst = ' '.join(syslst)
	print('Possible values for system are: %s' % syslst)


