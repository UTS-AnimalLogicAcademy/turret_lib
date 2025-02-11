# -*- coding: utf-8 -*-

name = 'turret_lib'

version = '1.6.7'

authors = [ 'ben.skinner',
            'daniel.flood',
	    'jonah.newton'
]

requires = ['libzmq-4',
            'cppzmq-4'
]

private_build_requires = [
    'cmake-3.12',
    'devtoolset'
]

variants = [
  ['platform-linux', 'arch-x86_64', 'tbb-2020.3', 'boost-1.76', '!tbb_katana','!boost_katana'], # maya 2023/Nuke 14.1
  ['platform-linux', 'arch-x86_64', 'tbb-2020.3', 'boost-1.82.0.1', '!tbb_katana','!boost_katana', 'python-3.11'], # maya 2025
  ['platform-linux', 'arch-x86_64', 'tbb-2020.3', 'boost-1.72.1', 'python-3.7', '!tbb_katana','!boost_katana', '!nuke'], # houdini 19.5 3.7
  ['platform-linux', 'arch-x86_64', 'tbb-2020.3', 'boost-1.72.1', 'python-3.9', '!tbb_katana','!boost_katana', '!nuke'], # houdini 19.5 3.9
  ['platform-linux', 'arch-x86_64', 'tbb-2020.3', 'boost-1.72.1', 'python-3.10', '!tbb_katana','!boost_katana', '!nuke'], # houdini 20
  ['platform-linux', 'arch-x86_64', 'tbb-2020.3', 'boost-1.82.0.0', 'python-3.10', '!tbb_katana','!boost_katana', '!nuke'], # houdini 20.5 py3.10
  ['platform-linux', 'arch-x86_64', 'tbb_katana-2020.3', 'boost_katana-1.76', '!tbb', '!boost', '!maya', '!nuke', '!blender'], #katana 6.5/mari 6
  ['platform-linux', 'arch-x86_64', 'tbb_katana-2020.3', 'boost_katana-1.80', '!tbb', '!boost', '!maya', '!nuke', '!blender'], #Katana 7/Mari 7
  ['platform-linux', 'arch-x86_64', 'tbb_katana-2020.3', 'boost_katana-1.82', '!tbb', '!boost', '!maya', '!nuke', '!blender'], #Katana 8
  ['platform-linux', 'arch-x86_64', 'tbb-2020.3', 'boost-1.80<1.80.0.2', '!maya', '!tbb_katana','!boost_katana', '!nuke'], #usd 23.08 standalone
  ['platform-linux', 'arch-x86_64', 'tbb-2020.3', 'boost-1.80.0.2', '!tbb_katana','!boost_katana', '!usd'],
  ['platform-linux', 'arch-x86_64', 'tbb-2020.3', 'boost-1.82.0.1', '!maya', '!tbb_katana','!boost_katana', '!nuke'], #usd 25.02 standalone
]

def commands():
    env.LIBTURRET_ROOT.set("{this.root}")
    env.LD_LIBRARY_PATH.append('{root}/lib')
    env.TURRET_RETRIES.set("1")
