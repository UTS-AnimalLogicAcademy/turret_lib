# -*- coding: utf-8 -*-

name = 'turret_lib'

version = '1.2.8'

authors = [ 'ben.skinner',
            'daniel.flood'
]

requires = ['libzmq-4',
            'cppzmq-4'
]

private_build_requires = [
    'cmake-3.12',
]

variants = [
  ['platform-linux', 'arch-x86_64', 'tbb-4', 'boost-1.55'], # older usd versions
  ['platform-linux', 'arch-x86_64', 'tbb-2017.0', 'boost-1.55', 'devtoolset-6'], # maya 2019
  ['platform-linux', 'arch-x86_64', 'tbb-2017.8', 'boost-1.61', 'devtoolset-6'],
  ['platform-linux', 'arch-x86_64', 'tbb_katana-2017', 'boost_katana-1.61'],
  ['platform-linux', 'arch-x86_64', 'tbb-2019.0', 'boost-1.61', 'devtoolset-6'],# houdini 18.0
  ['platform-linux', 'arch-x86_64', 'tbb-2019.9', 'boost-1.72', 'devtoolset-7'], # houdini 18.5
  ['platform-linux', 'arch-x86_64', 'tbb-2017.0', 'boost-1.61', 'devtoolset-6'] # maya 2022
]

def commands():
    env.LIBTURRET_ROOT.set("{this.root}")
    env.LD_LIBRARY_PATH.append('{root}/lib')
    env.TURRET_RETRIES.set("1")
