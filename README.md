# MyvarOS
A OS written in C

# Build / Run

``` ./build.py build kernel ``` This builds the kernel.

``` ./build.py run kernel ``` This first builds the kernel (if needed) then runs QEMU like normal

```./build.py run kernel -no-wait ``` This first builds the kernel (if needed) then runs QEMU but includes the no-wait argument for the serial port.

```./build.py run kernel -make-shell-script``` Will output the commands for a shell script, but it does not make use of environment variables and it does not include the ability to abort on errors.

