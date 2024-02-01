# -*- coding: utf-8 -*-

name = 'turret_lib'

version = '1.3.2'

authors = [ 'ben.skinner',
            'daniel.flood',
	    'jonah.newton'
]

requires = ['libzmq-4',
            'cppzmq-4'
]

private_build_requires = [
    'cmake-3.12',
    'devtoolset-7+'
]

variants = [
  ['platform-linux', 'arch-x86_64', 'tbb-4', 'boost-1.55', '!katana'], # older usd versions
  ['platform-linux', 'arch-x86_64', 'tbb-2017.0', 'boost-1.55', '!katana', '!nuke'], # maya 2019
  ['platform-linux', 'arch-x86_64', 'tbb-2020.3', 'boost-1.76', '!katana', '!nuke'], # maya 2023
  ['platform-linux', 'arch-x86_64', 'tbb-2017.8', 'boost-1.61', '!katana'],
  ['platform-linux', 'arch-x86_64', 'tbb_katana-2017', 'boost_katana-1.61', '!maya'],
  ['platform-linux', 'arch-x86_64', 'tbb-2019.0', 'boost-1.61', '!katana', '!maya', '!nuke'],# houdini 18.0
  ['platform-linux', 'arch-x86_64', 'tbb-2019.9', 'boost-1.72', '!katana', '!maya', '!nuke'], # houdini 18.5
  ['platform-linux', 'arch-x86_64', 'tbb-2020.3', 'boost-1.72', '!katana', '!nuke'], # maya-2022, usd-21.11
  ['platform-linux', 'arch-x86_64', 'tbb-2017.0', 'boost-1.61', '!katana', '!nuke'], # maya 2022, usd-20.11
  ['platform-linux', 'arch-x86_64', 'tbb_katana-2019.6', 'boost_katana-1.70', '!maya', '!nuke'], # katana 4.5v2 / katana 5
  ['platform-linux', 'arch-x86_64', 'tbb_katana-2020.3', 'boost_katana-1.76', '!maya', '!nuke'], #katana 6
  ['platform-linux', 'arch-x86_64', 'tbb-2018.4', 'boost-1.66', '!katana', '!maya'], #nuke 12.2v1
  ['platform-linux', 'arch-x86_64', 'tbb-2020.3', 'boost-1.80', '!maya', '!nuke', '!katana'] #usd 23.08 standalone
]

def commands():
    env.LIBTURRET_ROOT.set("{this.root}")
    env.LD_LIBRARY_PATH.append('{root}/lib')
    env.TURRET_RETRIES.set("1")
