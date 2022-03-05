# Userspace programs for the ITKernel

## Building

To prepare an `ustar` payload to upload to the ChipKIT, run `make` (i.e., `make all`).
This will compile the itklibc and the user programs, link them,
strip the binaries with Elfkickers's `sstrip` to reduce size,
and package them up into `payload.ustar`.

If you only want to compile the itklibc or the user programs
(e.g., if you want to only check that they compile),
simply run `make lib` or `make progs` respectively.

## Uploading

Run `make picocom` in this folder, select `load` on the ChipKIT menu,
then when prompted enter send mode from picocom (`Ctrl-a Ctrl-s` by default)
and enter `payload.ustar` (or the filename of the file you want to store).

If you want to read back what was written in the eeprom, enter receive mode from picocom
(`Ctrl-a Ctrl-r`) and enter the filename where you want the data to be saved,
then (and only after this step is complete and picocom is waiting for the data),
press button 4 on the ChipKIT to initiate the transfer;
if you don't care about this verification step, press button 2 to skip it.

## Prerequisites

Obviously you need the MIPS gcc toolchain installed, you need a working version of picocom,
and you need elfkickers's sstrip utility, although if you don't feel like installing it
just replace it with `touch` and live with the bloated binaries.

### Elfkickers

Easily compiled from source as follows (in a directory outside the project repo):

```sh
$ wget http://www.muppetlabs.com/~breadbox/pub/software/ELFkickers-3.2.tar.gz
$ tar xf ELFkickers-3.2.tar.gz
$ cd ELFkickers-3.2/
$ make
```

Then just put the absolute path of the generated sstrip binary in the Makefile
(if you just finished running the steps above, you can get the absolute path
by doing `echo $(pwd)/bin/sstrip`,
then replace `touch` in the line that says `SSTRIP = touch` with the path you got).

