# Pipe Viewer for the Forgetful (pvf)

`pvf` is a simple command-line utility designed to track the progress of a file being read by another process. It displays a progress bar showing how much of the file has been read.

## Features

- Monitors files being read by any process.
- Displays a dynamic progress bar.
- Optional watch mode for continuous monitoring.

## Prerequisites

- Linux operating system (requires access to `/proc` filesystem).
- Clang or GCC compiler.

## Building the Project

To build `pvf`, you need to have `make` and `clang` (or another compatible C compiler) installed on your system.

### Clone the Repository

```bash
git clone https://github.com/yourusername/pvf.git
cd pvf
Build
Run the following command to compile the project:
```
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
pvf works by inspecting the /proc filesystem to find processes that have the specified file open. It reads the file descriptor information to determine how much of the file has been read.

## Limitations
Only works on Linux systems.
Requires permissions to access /proc and the target file.
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
This project is licensed under the MIT License.
