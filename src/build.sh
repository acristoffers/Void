g++ -g -std=c++11 *.cpp \
    -I/usr/local/opt/nss/include/nss \
    -I/usr/local/opt/nspr/include/nspr \
    -I/usr/local/include \
    -L/usr/local/opt/nss/lib \
    -L/usr/local/opt/nspr/lib \
    -L/usr/local/lib \
    -lnss3 \
    -lnspr4 \
    -lscrypt
