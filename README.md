# Usage
```
git clone --recursive https://github.com/roy2220/dns-override
cd dns-override

＃ Generate ./libdns-override.so
./make

# Create overriding `hosts` file
echo 127.0.0.1 test.com > ./hosts

# Override /etc/hosts
./dns-override --hosts=./hosts -- curl -v test.com

# Create overriding `resolv.conf` file
echo nameserver 127.0.0.1 > ./resolv.conf

# Override /etc/resolv.conf
./dns-override --resolv-conf=./resolv.conf -- curl -v test.com
```
Note: If you test with the `ping` command, you must be the root, for the `ping` command playing some tricks.
