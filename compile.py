"""
GAPE YE, in AWE, of my SHITTY BUILDSCRIPT!

THOU shalt not TARNISH its majesty with thy Make nor thy Gradle!
"""

import os
from os.path import isdir, join
import subprocess as sp
import shutil

WORKING_DIR = os.getcwd()
projects = [f for f in os.listdir('.') if isdir(f) and f not in ('build', 'result')]

def clrdir(dr):
	shutil.rmtree(dr, True)
	os.mkdir(dr)

clrdir('build')
clrdir('result')

for proj in projects:
	projdir = join(WORKING_DIR, proj)
	outdir = join('build', proj)
	print('compiling %s' % proj)
	clrdir('build/%s' % proj)
	print('  making .o')
	for fullname in filter(lambda f: '.cpp' in f, os.listdir(projdir)):
		outpath = join(outdir, fullname[:-4] + '.o')
		fdir = join(proj, fullname)
		proc = os.system('g++ -o {buildpath} -c {fdir}'.format(buildpath=outpath, fdir=fdir))
	print('  bundling')
	os.system('g++ {outdir}/* -o result/{proj}.bin'.format(outdir=outdir, proj=proj))
