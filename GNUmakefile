#!/usr/bin/make -f
.SUFFIXES:

# dirname.out
TARGET := $(shell basename $$PWD).out

## make.mk
# commented assignments are overrides and are otherwise reasonable defaults.

CFLAGS   := -std=c23 -O2 -pthread
CXXFLAGS := -std=c++20 -O2 -pthread
CPPFLAGS := -D_GNU_SOURCE
LDFLAGS  := -lm

SOURCE.dir := source
OBJECT.dir := object

MAKE.dir := tool/make
MAKE.filter := 11-lib.mk 05-peru.mk

SEARCH.dir := ${SOURCE.dir} ${SOURCE.dir}/level-2

## compiler

# Externally overridable with CC=.. CXX=..
PREFER_GCC := 0

## verbose

# Megabroken, define here only if you want the `1' behavior,
# or inline or on command line.
# VERBOSE := 1

## debug

DEBUG          ?= 0
SANITIZE       ?= 0
VECTORIZED_ALL ?= 0
VECTORIZED     ?= 0
DO_LTO         ?= 1 		# always nulled if library

## peru

PERU_MUST_WORK := 0             # make peru nonoptional

## lib

NOT_APART_OF_LIBRARY :=
#LIBTARGET :=

## bison

LFLAGS += --debug --trace
YFLAGS += --debug

## gperf

# GPERF := gperf

## pch

HEADER.pch.filter :=

## dependency

#DEPEND := $(OBJECT.dir)/.depend

-include ${MAKE.dir}/make.mk
