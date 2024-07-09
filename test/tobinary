#!/bin/bash

fn=${1%.*}

llvm-as $1
llc ${fn}.bc
as -o ${fn}.o ${fn}.s
