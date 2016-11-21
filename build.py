import zipfile
import os,sys,re
import platform
import shutil

def unzip_file(zipfilename, unziptodir):
    if not os.path.exists(unziptodir): os.mkdir(unziptodir)
    zfobj = zipfile.ZipFile(zipfilename)
    for name in zfobj.namelist():
        name = name.replace('\\','/')

        if name.endswith('/'):
            os.mkdir(os.path.join(unziptodir, name))
        else:
            ext_filename = os.path.join(unziptodir, name)
            ext_dir= os.path.dirname(ext_filename)
            if not os.path.exists(ext_dir) : os.mkdir(ext_dir)
            outfile = open(ext_filename, 'wb')
            outfile.write(zfobj.read(name))
            outfile.close()

unpackpath = os.getcwd() + '\\' + '3rd' + '\\' + 'libevent-2.0.22-stable'
if os.path.exists(unpackpath):
    shutil.rmtree(unpackpath)

libeventpath = os.getcwd() + '\\' + '3rd' + '\\' + 'libevent-2.0.22-stable.zip'

print("unpack",libeventpath)
unzip_file(libeventpath, os.path.dirname(libeventpath))

if platform.system() == "Windows":
    print ("move file")
    srcdir = os.getcwd() + '\\' + '3rd' + '\\' + 'libevent-2.0.22-stable' + '\\' + 'WIN32-Code' + '\\' + 'tree.h'
    dstdir = os.getcwd() + '\\' + '3rd' + '\\' + 'libevent-2.0.22-stable' + '\\' + 'include' + '\\' + 'tree.h'
    shutil.copyfile(srcdir, dstdir)
    srcdir = os.getcwd() + '\\' + '3rd' + '\\' + 'libevent-2.0.22-stable' + '\\' + 'WIN32-Code' + '\\' + 'event2' + '\\' + 'event-config.h'
    dstdir = os.getcwd() + '\\' + '3rd' + '\\' + 'libevent-2.0.22-stable' + '\\' + 'include' + '\\' + 'event2' + '\\' + 'event-config.h'
    shutil.copyfile(srcdir, dstdir)

    os.system(os.getcwd() + '\\' + '3rd' + '\\' + 'libevent-2.0.22-stable' + '\\' + 'win_build.bat')

    libdpath = os.getcwd() + '\\' + 'LIB_D'
    if os.path.exists(libdpath) != True:
        os.mkdir(libdpath)

    shutil.copyfile(os.getcwd() + '\\' + '3rd' + '\\' + 'libevent-2.0.22-stable' + '\\' + 'libevent.lib', libdpath + '\\' + 'libevent.lib')
    shutil.copyfile(os.getcwd() + '\\' + '3rd' + '\\' + 'libevent-2.0.22-stable' + '\\' + 'libevent_core.lib', libdpath + '\\' + 'libevent_core.lib')
    shutil.copyfile(os.getcwd() + '\\' + '3rd' + '\\' + 'libevent-2.0.22-stable' + '\\' + 'libevent_extras.lib', libdpath + '\\' + 'libevent_extras.lib')

    buildpath = os.getcwd() + '\\' + 'build'
    if os.path.exists(buildpath) != True:
        os.mkdir(buildpath)

    os.system(os.getcwd() + '\\' + 'build.bat')

