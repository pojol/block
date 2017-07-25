import os
import stat

cur_path_ = os.getcwd() + '/3rd'
root_path = cur_path_[:cur_path_.rfind('/')]
print("root path ", root_path)
print("cur path", cur_path_)

def rmtree(top):
    for root, dirs, files in os.walk(top, topdown=False):
        for name in files:
            filename = os.path.join(root, name)
            os.chmod(filename, stat.S_IWUSR)
            os.remove(filename)
        for name in dirs:
            os.rmdir(os.path.join(root, name))
    os.rmdir(top)


###################libevent###################################
print("download Libevent version release-2.0.22-stable")

os.chdir(cur_path_)

if os.path.exists(cur_path_ + "/Libevent"):
    rmtree(cur_path_ + "/Libevent")

os.system('git clone https://github.com/nmathewson/Libevent.git')
os.chdir(cur_path_ + "/Libevent")
os.system('git checkout release-2.0.22-stable')
os.system('./autogen.sh')
os.system('./configure --prefix=/' + root_path + '/lib/libevent')
os.system('make')
os.system('make install')

os.chdir(cur_path_)

#################glog####################################
print("download glog version 0.3.4")

if os.path.exists(cur_path_ + "/glog"):
    rmtree(cur_path_ + "/glog")

os.system('git clone https://github.com/google/glog.git')
os.chdir(cur_path_ + "/glog")
os.system('git checkout v0.3.4')
os.system('./configure --prefix=/' + root_path + '/lib/glog')
os.system('make')
os.system('make install')
os.chdir(cur_path_)


###################protobuf############################
print("download protobuf version 3.2.0")

if os.path.exists(cur_path_ + "/protobuf"):
    rmtree(cur_path_ + "/protobuf")

os.system('git clone https://github.com/google/protobuf.git')
os.chdir(cur_path_ + '/protobuf')
os.system('git checkout v3.2.0')
os.system('./autogen.sh')
os.system('./configure --prefix=/' + root_path + '/lib/protobuf')
os.system('make')
os.system('make install')
os.chdir(cur_path_)


##################sol##############################
print("download sol2 version 2.16.0")

if os.path.exists(cur_path_ + "/sol2"):
    rmtree(cur_path_ + "/sol2")

os.system('git clone https://github.com/ThePhD/sol2.git')
os.chdir(cur_path_ + '/sol2')
os.system('git checkout v2.16.0')
os.chdir(cur_path_)


#########################hiredis##############################
print("download hiredis version 0.13.3")

if os.path.exists(cur_path_ + "/hiredis"):
    rmtree(cur_path_ + "/hiredis")
os.system('git clone https://github.com/redis/hiredis.git')
os.chdir(cur_path_ + '/hiredis')
os.system('git checkout v0.13.3')
os.system('make')
os.system('cp -r ' + cur_path_ + '/hiredis' + ' ' + root_path + '/lib/hiredis')
os.chdir(cur_path_)

######################lua############################
print("download lua version 5.3.0")

if os.path.exists(cur_path_ + "/lua-5.3.4"):
    rmtree(cur_path_ + "/lua-5.3.4")

os.system('wget -P ' + root_path + '/3rd http://www.lua.org/ftp/lua-5.3.4.tar.gz')
os.system('tar -xzvf ./lua-5.3.4.tar.gz')
os.chdir(cur_path_ + '/lua-5.3.4')
os.system('make linux')
os.system('mkdir ' + root_path + '/lib/lua')
os.system('cp -r ' + cur_path_ + '/lua-5.3.4/src/* ' + ' ' + root_path + '/lib/lua/')

os.chdir(root_path)