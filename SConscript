from building import *
import os

cwd     = GetCurrentDir()
CPPPATH = [cwd + '/include']
CPPPATH += [cwd + '/src']
CPPPATH += [cwd + '/src/common']

group = []

src = Glob('src/common/*.c')
SrcRemove(src, ['src/common/pf_snmp.c'])

group_common = DefineGroup('p-net/common', src, depend = ['PKG_USING_PNET'], CPPPATH = CPPPATH)

CPPPATH = [cwd + '/src/device']
src = Glob('src/device/*.c')

group_device = DefineGroup('p-net/device', src, depend = ['PKG_USING_PNET'], CPPPATH = CPPPATH)

CPPPATH = [cwd + '/osal/include']
CPPPATH += [cwd + '/osal/src/rtthread']
src = Glob('osal/src/rtthread/*.c')

group_osal = DefineGroup('p-net/osal', src, depend = ['PKG_USING_PNET'], CPPPATH = CPPPATH)

CPPPATH = [cwd + '/src/ports/rtthread']
CPPPATH += [cwd + '/src/ports/rtthread/pnal']

src = src + Glob('src/ports/rtthread/pnal/*.c')
SrcRemove(src, ['src/ports/rtthread/pnal/pnal_snmp.c'])

group_port = DefineGroup('p-net/port', src, depend = ['PKG_USING_PNET'], CPPPATH = CPPPATH)

CPPPATH = [cwd + '/src/ports/rtthread/pn_dev']
src = Glob('src/ports/rtthread/pn_dev/*.c')

group_sample = DefineGroup('p-net/sample', src, depend = ['PKG_USING_PNET'], CPPPATH = CPPPATH)

group = group_common + group_device + group_port + group_sample

Return('group')
