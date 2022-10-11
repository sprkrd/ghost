#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from sys import argv


tr = dict(zip("åáàäâéèëêíìïîóòöôúùüûñç", "aaaaaeeeeiiiioooouuuu~$"))

dict_file = argv[1]
out_file = argv[2]
with open(dict_file, "r") as in_, open(out_file, "w") as out:
    for word in in_:
        if "'" in word:
            continue
        word = word.lower()
        for k,v in tr.items():
            word = word.replace(k,v)
        out.write(word)

