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
	print('cmdline', cmdline)
	if 'MAKE_SHELL_SCRIPT' in os.environ and os.environ['MAKE_SHELL_SCRIPT'] == '1':
		print(cmdline)
		return (b'', b'')
	proc = subprocess.Popen(cmdline, shell=True, stderr=stderr, stdout=stdout)
	return proc.communicate(timeout=comm_timeout)

def copyfile(src, dst):
	if 'MAKE_SHELL_SCRIPT' in os.environ and os.environ['MAKE_SHELL_SCRIPT'] == '1':
		print('cp %s %s' % (src, dst))
		return
	shutil.copyfile(src, dst)

def action_build(copts):
	if copts.system[0] != 'kernel':
		print('Only supported system to build is "kernel".')
		exit(-1)

	if os.path.exists('./bin') is False:
		os.makedirs('./bin')

	objs = []

	base_dir = os.path.abspath('./')

	for src in config.sources:
		ignore_stderr = False

		if type(src) is dict:
			if 'ignore_stderr' in src and src['ignore_stderr'] is True:
				ignore_stderr = True
			src = src['src']

		fsrc = os.path.abspath(src)
		(bsrc, esrc) = os.path.splitext(fsrc)

		(_, src) = os.path.split(fsrc)

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

		if 'VERBOSE' in os.environ and os.environ['VERBOSE'] == '1':
			print(' '.join(opts))

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

	if 'VERBOSE' in os.environ and os.environ['VERBOSE'] == '1':
		print(' '.join(opts))

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

	if 'VERBOSE' in os.environ and os.environ['VERBOSE'] == '1':
		print(' '.join(opts))

	try:
		run(' '.join(opts), stdout=sys.stdout, stderr=sys.stderr, comm_timeout=None)
	except:
		pass

def main(args):
	if 'NASM_BIN' not in os.environ:
		os.environ['NASM_BIN'] = config.NASM_BIN
	if 'LD_BIN' not in os.environ:
		os.environ['LD_BIN'] = config.LD_BIN
	if 'GCC_BIN' not in os.environ:
		os.environ['GCC_BIN'] = config.GCC_BIN
	if 'ISO_BIN' not in os.environ:
		os.environ['ISO_BIN'] = config.ISO_BIN
	if 'QEMU_BIN' not in os.environ:
		os.environ['QEMU_BIN'] = config.QEMU_I386_BIN

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


main(sys.argv)

