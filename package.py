# -*- coding: utf-8 -*-

name = 'turret_lib'

version = '0.0.3'

authors = [ 'ben.skinner',
            'daniel.flood'
          ]

requires = ['libzmq-4',
            'cppzmq-4',
            'boost-1.55'
           ]

build_requires = [
    'cmake-3.2',
]

variants = [['platform-linux', 'arch-x86_64']]

def commands():
    env.LIBTURRET_ROOT.set("{this.root}")
    env.LD_LIBRARY_PATH.append('{root}/lib')
