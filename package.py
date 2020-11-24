# -*- coding: utf-8 -*-

name = 'turret_lib'

version = '1.2.0'

authors = [ 'ben.skinner',
            'daniel.flood'
]

requires = ['libzmq-4',
            'cppzmq-4'
]

build_requires = [
    'cmake-3.2',
]

variants = [
  ['platform-linux', 'arch-x86_64', 'tbb-4', 'boost-1.55'],
  ['platform-linux', 'arch-x86_64', 'tbb-2019', 'boost-1.55'],
  ['platform-linux', 'arch-x86_64', 'tbb_katana-2017', 'boost_katana-1.61'],
]

def commands():
    env.LIBTURRET_ROOT.set("{this.root}")
    env.LD_LIBRARY_PATH.append('{root}/lib')
    env.TURRET_RETRIES.set("1")
