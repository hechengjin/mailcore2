mailcore2编译：
Build on Linux

Install the following debian packages:
sudo apt-get install libctemplate-dev libicu-dev libsasl2-dev libtidy-dev \
    uuid-dev libxml2-dev libglib2.0-dev autoconf automake libtool
Grab and compile the latest of libetpan: https://github.com/dinhviethoa/libetpan
mkdir ~/libetpan
cd ~/libetpan
git clone --depth=1 https://github.com/dinhviethoa/libetpan
cd libetpan
./autogen.sh
make >/dev/null
sudo make install prefix=/usr >/dev/null
Compile MailCore 2:
cd ~/mailcore2
mkdir build
cd build
cmake ..
make


相关问题：
1.mailcore2/src/core/security/MCCertificateUtils.cpp:15:25: fatal error: openssl/bio.h: 没有那个文件或目录
 #include <openssl/bio.h>
mailcore2/src/core/security/openssl/e_os2.h:13:34: fatal error: openssl/opensslconf.h: 没有那个文件或目录
 # include <openssl/opensslconf.h>

创建下面文件夹
mailcore2/Externals/include
执行 git clone https://github.com/openssl/openssl.git

编译openssl 参数 https://github.com/openssl/openssl/blob/master/INSTALL

进入 mailcore2/Externals/include/openssl
		$ ./config
    $ make      ---这一步执行完后 就会有 opensslconf.h.in 转为 opensslconf.h 文件
    $ make test
    $ make install

然后 把第二步生成的 mailcore2/Externals/include/openssl/include/openssl 文件夹复制到 mailcore2/src/core/security 文件夹中

2./usr/bin/ld: 找不到 -lssl     
/usr/bin/ld: 找不到 -lcrypto

【原因】：
/lib 和 /lib64 或 /usr/local/lib 找不到libssl.so 和 libcrypto.so
解决方法：在安装openssl时，执行第4步， make install
安装后，就会在下面目录下找到相应文件，如：
/usr/local/lib/libssl.so
/usr/local/lib/libssl.so.1.1
