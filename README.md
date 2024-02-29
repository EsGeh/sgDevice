# sgDevice

*sgDevice* is a MIDI interface that can send and receive MIDI messages to/from a computer via USB.
It has a knobs, switches and triggers. The state of these controls is reported to the connected computer.
It may therefore be used as an input device to control *any kind* of software that receives MIDI messages - there is no intrinsic audio related functionality.
sgDevice (ver. 2) uses the [Arduino](https://www.arduino.cc/) Mega microprocessor board.

This repository contains:

- Basic information about *sgDevice*, a homemade MIDI device

- A software library for the [puredata](https://puredata.info/) music software that enables puredata patches to react to messages from the sgDevice. This allows a pd patch to e.g. play notes when a trigger is pressed or updating an effect parameter based on a knob position

- Tools and utility scripts to compile and upload the firmware to the sgDevice

## Pictures

![pd plus device](img/small/pd-plus-device.jpg)
![sgDevice ver 2](img/small/20190827_135715.jpg)
![sgDevice ver 2](img/small/20190827_140000.jpg)
![sgDevice ver 2, backside](img/small/20190827_140756.jpg)
![sgDevice ver 2, interior](img/small/20190822_131132.jpg)

## Prerequisits

- [fish shell](https://fishshell.com/)

## Project setup

- Initialise project (download dependencies, ...):

		$ ./scripts/init.fish
	
	This will download and git dependencies into `./dependencies/`

- Clean project

		$ ./scripts/exit.fish

# Build and upload Firmware

## Build Dependencies

- [arduino-cli](https://github.com/arduino/arduino-cli)
- ( legacy) `arduino`: only for sgDevice1

## sgDevice2

- Turn the Arduino into a native MIDI device:

    The case has to be open in order to set a jumper.
    This (hopefully) has only to be done once!

    plug the device in the PC via USB, then run:

        $ ./scripts/arduino_upload_usb_firmware.fish

    The script will guide you

- Set midi device name:

    plug the device in the PC via USB, then run:

        $ ./scripts/midi_set_device_name.fish

## sgDevice1 (legacy)

Plug the device in the PC via USB, then run:

    $ ./scripts/arduino_upload.fish

The script will guide you

# Build and install the Puredata Library for sgDevice

## Runtime Dependencies

- [puredata](https://puredata.info/): sgDevice is a midi controller to be used with puredata.
- [zexy](https://git.iem.at/pd/zexy)
- [structuredData](https://github.com/EsGeh/structuredData): Utiliy library for puredata

## Test locally without installing to system

1. Install library to a local dir

		$ ./scripts/local_init.fish

2. Run Example

		$ ./scripts/local_run.fish

	This opens an example patch documenting the major parts of this library.

3. Uninstall library from the local dir

		$ ./scripts/local_exit.fish

## Installation

- Build and install:

        $ ./scripts/build.fish build install

    (for options append `--help`)

- Install the dependencies: structuredData, zexy

    Please refer to the corresponding documentation for these libraries.

- Add to the pd library search path:

	You have two options:

	- Start pd from the command line like this:

			$ pd -lib sgDevice

	- Add this to `~/.pdsettings` :

			...
			loadlib1: sgDevice
			...

	    (manually adjust the numbering!)

## Documentation

The Documentation is provided as puredata example patches.
When the library is correctly installed to pd, the `[sgDevice-help]` patch gives an overview of the relevant objects and how they are used.
Right clicking and selecting "help" opens an example patch for most objects.

# Wiring Schema

![sgDevice 2 wiring schema](img/sgDevice2_wiring.svg)

# Testing and Debugging

## sgDevice2 - Test native Midi Device

	$ ./scripts/local_init.fish
	$ ./scripts/local_run.fish --debug

Check `./logs/` directory afterwards

## sgDevice1 - Test USB input (legacy)

	$ ./scripts/arduino_usbdump.fish

# References

- MIDI Specification:

	- <http://midi.teragonaudio.com/tech/midispec.htm>

- [USBMidiKliK](https://github.com/EsGeh/USBMidiKliK.git):

	Turn the Arduino into a native MIDI Device.
