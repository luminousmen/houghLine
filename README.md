Hough lines
=====

University course work. You can use it as example for your studies.

This program implements standart Hough transform algorithm. But it's not uses opencv [hough lines function](http://docs.opencv.org/3.0-beta/doc/py_tutorials/py_imgproc/py_houghlines/py_houghlines.html), I do it manualy. OpenCV library needed for standart functions.

### Dependencies

* [opencv >= 3.0](https://github.com/Itseez/opencv)


### Algorithm

[Wiki](https://en.wikipedia.org/wiki/Hough_transform)


### Usage

```bash
$ make
$ ./prog -i lines.png -l 400 
```

### Result

![result](/res.png)
