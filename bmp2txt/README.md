### bmp2txt
Trasform a bmp image in a colored script that can be show in CLI.

### Compile
Compile the c file:
```
gcc bmp2txt.c
```

### Usage
The program accept:
- BMP image path
- target width (about the width we want to show it in terminal)
- target heigth (about the heigth we want to show it in terminal)
- character width (width of a character in terminal)
- character heigth (heigth of a character in terminal)
And output text that is a script showing the image

It must be used in this way:
```
./a.out myimage 70 100 10 19 > script.sh
chmod 755 script.sh
./script.sh
```

![image](https://user-images.githubusercontent.com/5327501/211430986-5e0ce4d6-cec5-413d-b1f7-8312d4f152aa.png)
