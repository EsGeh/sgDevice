# sgDevice

software utilities to use my homemade midi input device with puredata

## Build Dependencies

- fish shell
- autotools
- arduino

## Preparing the Repository, and cleaning

- Make shure to issue this command before any other:

		$ ./scripts/init.fish
	
	This will download git repositories and dependencies and install them into the file hierarchy.

- Cleaning the file hierarchy:

		$ ./scripts/exit.fish

## Build and Install

- build and install the software on sgDevice (arduino part):

	plug the device in the PC via USB, then run:

		$ ./scripts/arduino_upload.fish

- build and install the PC part (puredata library):

		$ ./scripts/build.fish build install
	
	When running pd, you have 2 options:

	- start pd from the command line like this:

			$ pd [-path <install_path>] -lib sgDevice

	- add this to `~/.pdsettings` :

			...
			loadlib1: sgDevice
			...

		you might also have to adjust the library search path:

			...
			path1: <install_path>
			...

	(manuall adjust the numbering!)

## Documentation

### init docu

	$ ./scripts/doc_init.fish

### run docu

	$ ./scripts/doc_run.fish

### cleanup docu

	$ ./scripts/doc_exit.fish
