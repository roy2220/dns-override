# Usage
```
＃ Generate libresolvconf-overrider.so
./make

# Create overriding resolv.conf
echo nameserver 127.0.0.1 > ./resolv.conf

# Test
LD_PRELOAD=./libresolvconf-overrider.so PATH_RESCONF=./resolv.conf curl google.com
```
