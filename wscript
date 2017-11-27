#!/usr/bin/env python
# encoding: utf-8

# the following two variables are used by the target "waf dist"
VERSION = '0.0.1'
APPNAME = 'cxx_test'

# these variables are mandatory ('/' are converted automatically)
top = '.'
out = 'build'


def options(opt):
    opt.load('compiler_cxx')
    opt.load('default-compiler-flags boost cryptopp',
             tooldir='.waf-tools')


def configure(conf):
    conf.load('compiler_cxx default-compiler-flags boost cryptopp')

    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'],
                   uselib_store='NDN_CXX', mandatory=True)

    conf.check_cryptopp()

    boost_libs = 'system filesystem program_options'
    conf.check_boost(lib=boost_libs)

    conf.check_compiler_flags()


def build(bld):
    bld.program(source='src/consumer/main.cpp src/consumer/consumer.cpp',
                target='bin/consumer',
                use='NDN_CXX BOOST CRYPTOPP')

    bld.program(source='src/producer/main.cpp src/producer/producer.cpp',
                target='bin/producer',
                use='NDN_CXX BOOST CRYPTOPP')
