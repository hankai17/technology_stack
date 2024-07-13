#!/usr/bin/python3
#coding=utf8

import matplotlib.pyplot as plt
import numpy as np
import sympy as sp

def test():
    x = sp.symbols('x')
    y = x ** 2
    ds = sp.diff(y, x)
    print(ds)
    res = ds.evalf(subs = {x: 1/2})
    print(res)

def test1():
    x = sp.symbols('x')
    y = 2 * sp.exp(x) - x * sp.sin(x)
    ds_1 = sp.diff(y, x)
    ds_2 = sp.diff(y, x, 2)
    value = ds_2.evalf(subs = {x: 0})
    print(ds_1)
    print(ds_2)
    print(value)

def test2():
    x, y = sp.symbols('x y')
    z = sp.sin(y) + sp.exp(x) - x * y ** 2
    ds = -sp.diff(z, x) / sp.diff(z, y)
    print(ds)

def test3():
    x = sp.symbols('x')
    f = x ** 3 - 3 * x
    ds = sp.diff(f, x)
    ans = sp.solve(ds, x)
    print(ds)
    print(ans)

#test()
#test1()
#test2()
test3()
