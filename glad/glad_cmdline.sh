#!/bin/sh
#This is the command used to generate the loader

python -m glad --profile core --out-path out --generator c --api gl=3.3,gles2=3.0 --local-files --omit-khrplatform --extensions ""
