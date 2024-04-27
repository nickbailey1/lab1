## UID: 305933481

## Pipe Up

This program mimics the behavior of piping programs in Unix systems.

## Building

The executable file can be built using the following command which executes the make routine written in Makefile:
```make```

## Running

The program can be run using the following syntax:

```./pipe exec1 exec2 exec3 ... execN```

where each argument is a program. The output of the first program will be used as input for the next, and so on. pipe does not support arguments for the individual programs.

Examples:
```./pipe ls cat wc```
```./pipe ls cat```
```./pipe ls```
```./pipe ps wc```

## Cleaning up

Binaries can be cleaned up using the following command:
```make clean```
