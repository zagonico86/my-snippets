# Pi

## JAVA
Compile with:
```
javac PiComp.java
```
Launch with:
```
# use big integer in computation, for 1000 digits:
java PiComp binBig 1000
# use arrays in computation, for 1000 digits:
java PiComp binCust 1000
```
## C
Compile with:
```
gcc -O2 -o pi pi.c
```
Launch with:
```
# 10000 digits using formula pi/4 = 4 arctan 1/5 - arctan 1/239
./pi 1 10000
# 10000 digits using formula pi/4 = 64 arctan 1/57 + 7 arctan 1/239 -  12 arctan 1/682 + 24 arctan 1/12943
./pi 2 10000
```

See [https://www.zagonico.com/one-million-digits-of-pi/](https://www.zagonico.com/one-million-digits-of-pi/) for details.


