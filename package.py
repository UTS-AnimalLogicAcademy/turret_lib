# -*- coding: utf-8 -*-

name = 'turret_lib'

version = '0.0.9'

authors = [ 'ben.skinner',
            'daniel.flood'
          ]

requires = ['libzmq',
            'boost-1.55'
           ]

build_requires = [
    'cmake-3.2',
]

variants = [['platform-linux', 'arch-x86_64']]

def commands():
    env.TURRET_CLIENT_INCLUDE.set("{root}/include")
    env.TURRET_CLIENT_LIB.set("{root}/lib")
    env.LD_LIBRARY_PATH.append('{root}/lib')
