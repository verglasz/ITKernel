# ITKernel

This code was written as the project for a \`\`Computer Hardware Engineering'' course
(which basically means low-level C and assembly, and some CPU architecture stuff).
It runs _very specifically_ on Digilent's [uC32 Chipkit][uc32] with the [basic IO shield][iosh]
(or, well, I guess in some sense it runs on Microchip's [PIC32MX370F512H][mx32] microcontroller,
but it essentially assumes the Chipkit's and shield's IO configuration... I mean,
technically I suppose one could say it runs on a Mips32r2 4K core,
but I don't know what it would do on a different chip except crash
at boot since it hardcodes basically everthing, including eg memory configuration).

Calling it a kernel is ~~a blatant lie~~ very generous, but it lets us make an [ITK][itk] pun
so the name stays.
Essentially it can communicate through serial interface
(UART on the chip's side, but there's a UART-to-USB module on the Chipkit)
for diagnostics and for loading up files in the EEPROM (provided by the IO Shield)
through a very scuffed interface (basically we use `picocom` and send files byte
by byte through it with the aid of a Python script (which is tuned to send files
in exactly the way the ITKernel code expects to receive them (what do you mean
\`\`this is not how you build a reliable embedded system''??));
then it can load statically-linked (with a custom linker script) executables
and run them in usermode, offering a handful of syscalls.
By which I mean, like, 5.
Including `exit`.

Ideally I'd polish this quite a bit more, add some reasonable error handling and such,
but realistically it's a bit pointless (and also, originally I wanted to make this
in Rust but the course didn't allow it, so if I come back to this it's more likely
it'll be to RiiR)... but if I do add to this,
it would also be cool to fix the problems with loading to the Chipkit
code that uses high physical memory addresses (for some reason we're stuck to
well below the total flash available), and to actually write a dynamic linker so
we could offer a system `libc` and have smaller, dynamically linked binaries.

[uc32]: https://digilent.com/reference/microprocessor/uc32/start
[iosh]: https://digilent.com/reference/add-ons/basic-io-shield/start
[mx32]: https://www.microchip.com/en-us/product/PIC32MX370F512H
[itk]: https://www.facebook.com/ITerativaKlubben

