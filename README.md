# A project to implement a connection between two COM ports.

### Requirements:
- For the project to work, it is required to organize a physical connection between two COM ports. For example, between two USB-to-COM bridges connected.
- Before compiling the program, you must specify the actual names of the ports issued by the system for the `rComPortName` and `wComPortName` variables.

### Work description:
One of the two ports will be assigned as a transmitter, the other as a receiver. The transmitter will send a set of bytes (**"Hello world!"** in ASCII characters) that the other port will receive.
