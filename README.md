# Pipe Viewer for the Forgetful (pvf) FreeBSD version

`pvf` is a simple command-line utility designed to track the progress of a file being read by another process. It displays a progress bar showing how much of the file has been read.

## Features

- Monitors files being read by any process.
- Displays a dynamic progress bar.
- Optional watch mode for continuous monitoring.

## Prerequisites

- FreeBSD operating system
- Clang or GCC compiler.

## Building the Project

To build `pvf`, you need to have `make` and `clang` (or another compatible C compiler) installed on your system.

### Clone the Repository

```bash
git clone https://github.com/willingly-or-unwillingly/pvff.git
cd pvf
```
## Build
Run the following command to compile the project:

```bash
make
```
This will create the executable in the bin directory.

## Installation

```bash
sudo make install
```
This will copy the binary to /usr/local/bin.

## Uninstallation
To remove pvf from your system:

```bash
sudo make uninstall
```

## Usage
The basic syntax for using pvf is:

```bash
pvf [--watch=NNN | -w NNN] <filename>
````
filename: The file you want to monitor.
--watch=NNN or -w NNN: Optional argument to specify a watch interval in seconds for continuous monitoring.

## Example
Monitor a file once:

```bash
pvf /path/to/file.txt
```
Monitor a file continuously every 5 seconds:

```bash
pvf --watch=5 /path/to/file.txt
```
Progress Bar
The progress bar displays the percentage of the file read and a spinner to indicate activity:
```bash
[####################                    ] 50.00% /
```

## How It Works

## Limitations
Only works on FreeBSD systems.

## Troubleshooting
File is not open by any process or cannot be accessed: Ensure the file is currently being read by a process and that you have the necessary permissions.

File size is zero or cannot be accessed: Verify the file exists and is non-empty.

## Cleaning Up
To clean up the build files:

```bash
make clean
```

## Contributing
Contributions are welcome! Please open an issue or submit a pull request on GitHub.

## License
This project is licensed under the BSD License.
