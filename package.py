# -*- coding: utf-8 -*-

name = 'zmq_client_cpp'

version = '0.0.1'

authors = [ 'ben.skinner' ]

requires = ['libzmq']

build_requires = [
    'cmake-3.2',
]

variants = []

def commands():
    env.ZMQ_CLIENT_INCLUDE.set("{root}/include")
    env.ZMQ_CLIENT_LIB.set("{root}/lib/libzmq_client_cpp.so")

    env.ZMQ_CLIENT_LOG_LEVEL.set("1")
