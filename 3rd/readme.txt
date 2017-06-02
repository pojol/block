网络库
libevent
https://github.com/libevent/libevent/releases/download/release-2.0.22-stable/libevent-2.0.22-stable.tar.gz


chmod 777 configure
./configure --prefix=/xx/gsf/lib/linux/libevent
make && make install


日志库，当前用master版本（因为可以通过cmake构建
glog
https://github.com/google/glog.git

git clone https://github.com/google/glog.git
./autogen.sh 
./configure --prefix=/xx/gsf/lib/linux/glog
make && make install


描述，压缩，解析源数据，用于网络传输和数据存储。
protobuf
https://github.com/google/protobuf/tree/v3.2.0


c++ & lua 绑定器
sol
https://github.com/ThePhD/sol2/tree/v2.16.0


git clone https://github.com/ThePhD/sol2.git
git checkout v2.16.0


lua 5.3

wget -P /home/xx/gsf/3rd/lua53 http://www.lua.org/ftp/lua-5.3.4.tar.gz
tar -xzvf ./lua-5.3.4.tar.gz
make linux
cp liblua.a /home/xx/gsf/lib/linux/

pbc编译

git clone https://github.com/owent-contrib/pbc.git
git checkout proto_v3
window --
建立build目录 ，cmake .. 编译出pdb.lib
将gsf/3rd目录的 pbc-lua53.c 编译成pdb_lua.lib 