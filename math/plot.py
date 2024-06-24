#!/usr/bin/python3
#coding=utf8

##!/usr/bin/env python

import matplotlib.pyplot as plt
from numpy import *


def test1():
    x = arange(0, 2 * pi, 0.01)     # 定义0~2pi 公差为0.01的数组
    y1 = sin(x)
    y2 = cos(x)
    
    plt.figure()
    #plt.plot(x, y1, x, y2)
    plt.plot(x, y1, color='r', linestyle='-', label='sin(x)')
    plt.plot(x, y2, color='b', linestyle='-.', label='cos(x)')
    plt.legend()
    plt.show()

def test2():
    x = arange(-5, 5, 0.01)
    y = sin(x) + cos(x) + 1
    plt.figure()
    plt.plot(x, y)
    plt.axis([-6, 6, -3, 3])
    plt.grid(True)
    plt.show()


def test3():
    x = arange(-5, 5, 0.01)
    y = log2(x + sqrt(1 + x ** 2))
    plt.figure()
    plt.plot(x, y)
    plt.grid(True)
    plt.show()

def test4():
    x = arange(-5 * pi, 5 * pi, 0.01)
    y = sin(x) / x
    plt.figure()
    plt.plot(x, y)
    plt.grid(True)
    plt.show()




#test1()
#test2()
#test3()
test4()
