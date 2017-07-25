import os
import stat

cur_path_ = os.getcwd()

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

root_path = cur_path_[:cur_path_.rfind('/')]
print("root path ", root_path)

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
'''
#################glog####################################
print("download glog version 0.3.4")

if os.path.exists(cur_path_ + "/glog"):
    rmtree(cur_path_ + "/glog")

os.system('git clone https://github.com/google/glog.git')
os.chdir(cur_path_ + "/glog")
os.system('git checkout v0.3.4')
os.chdir(cur_path_)


###################protobuf############################
print("download protobuf version 3.2.0")

if os.path.exists(cur_path_ + "/protobuf"):
    rmtree(cur_path_ + "/protobuf")

os.system('git clone https://github.com/google/protobuf.git')
os.chdir(cur_path_ + '/protobuf')
os.system('git checkout v3.2.0')
os.chdir(cur_path_)


##################sol##############################
print("download sol2 version 2.16.0")

if os.path.exists(cur_path_ + "/sol2"):
    rmtree(cur_path_ + "/sol2")

os.system('git clone https://github.com/ThePhD/sol2.git')
os.chdir(cur_path_ + '/sol2')
os.system('git checkout v2.16.0')
os.chdir(cur_path_)


######################lua############################
print("download lua version 5.3.0")

if os.path.exists(cur_path_ + "/lua"):
    rmtree(cur_path_ + "/lua")

os.system('git clone https://github.com/LuaDist/lua.git')
os.chdir(cur_path_ + '/lua')
os.system('git checkout 5.3.0')
os.chdir(cur_path_)


##################pbc################################
print("download pbc version proto_v3")

if os.path.exists(cur_path_ + '/pbc'):
    rmtree(cur_path_ + '/pbc')

os.system('git clone https://github.com/owent-contrib/pbc.git')
os.chdir(cur_path_ + '/pbc')
os.system('git checkout proto_v3')
os.chdir(cur_path_)
'''


'''
  - cd 3rd
  - git clone https://github.com/google/glog.git
  - cd glog
  - git checkout v0.3.4
  - ./configure --prefix=/home/travis/build/pojol/gsf/lib/glog
  - make && make install

  - cd /home/travis/build/pojol/gsf/3rd
  - git clone https://github.com/nmathewson/Libevent.git
  - cd Libevent
  - git checkout release-2.0.22-stable
  - ./autogen.sh
  - ./configure --prefix=/home/travis/build/pojol/gsf/lib/libevent
  - make && make install

  - cd /home/travis/build/pojol/gsf/3rd
  - git clone https://github.com/google/protobuf.git
  - cd protobuf
  - git checkout v3.2.0
  - ./autogen.sh
  - ./configure --prefix=/home/travis/build/pojol/gsf/lib/protobuf
  - make
  - make install

  - cd /home/travis/build/pojol/gsf/3rd
  - git clone https://github.com/ThePhD/sol2.git
  - cd sol2
  - git checkout v2.16.0

  - cd /home/travis/build/pojol/gsf/3rd
  - git clone https://github.com/redis/hiredis.git
  - cd hiredis
  - git checkout v0.13.3
  - make
  - cp -r /home/travis/build/pojol/gsf/3rd/hiredis /home/travis/build/pojol/gsf/lib

  - cd /home/travis/build/pojol/gsf/3rd
  - wget -P /home/travis/build/pojol/gsf/3rd http://www.lua.org/ftp/lua-5.3.4.tar.gz
  - tar -xzvf ./lua-5.3.4.tar.gz
  - cd lua-5.3.4
  - make linux
  - mkdir /home/travis/build/pojol/gsf/lib/lua
  - cp -r /home/travis/build/pojol/gsf/3rd/lua-5.3.4/src/* /home/travis/build/pojol/gsf/lib/lua/
'''