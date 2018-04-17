# y.modal

>  A Max external for modal synthesis.

The external creates banks of two pole resonators, and provides a series of functionalities to manipulate the banks.

## Installing

The repository is structured as a [Max package](https://docs.cycling74.com/max7/vignettes/packages). To use the external in Max, simply copy the repository into one of the two possible Max package folders. With Max 7, these would be:

- `Max 7/Packages` in your Documents (Mac) or My Documents (Windows) folder, or
- `Max 7/Packages` in your `/Users/Shared` (Mac) or `C:\ProgramData` (Windows) folder

## Compiling

While compiled externals are available in the package, you can also recompile them using Xcode on Mac, or Visual Studio on Windows. You will need a current version of the [Max SDK](https://cycling74.com/downloads/sdk). The externals are built in [externals/](externals/).

### XCode

See the xcode project in [build/osx-xcode/](build/osx-xcode/).

### Visual Studio

See the Visual Studio project and solution in [build/win-vs/](build/win-vs/). The Max SDK should be placed in the same parent folder as the repository, and its folder renamed to `max-sdk`:

```
Packages ──┬── max-sdk
           └── y.modal
```

For a different folder structure, change the macro that indicates the path to the Max SDK in [max_extern_common.props](build/win-vs/max_extern_common.props), which is currently set to:

```
<C74_SDK>$(ProjectDir)..\..\..\max-sdk\</C74_SDK>
```

## Usage in Max

### Inlets

0. All purpose inlet for messages

### Outlets

0. Audio output channel 1
1. Audio output channel 2
2. Audio output channel 3
3. Audio output channel 4
4. Audio output channel 5
5. Audio output channel 6
6. Audio output channel 7
7. Audio output channel 8
8. All purpose outlet for messages
9. Float outlet to monitor the rms of one resonator

### Arguments

Up to three arguments can be provided, in the following order. Default values are substituted for arguments that are omitted.

- The number of resonator banks (Default = 10)
- The maximum number of resonators per bank (Default = 150)
- The maximum number of states (Default = 10)

### Attributes

- **smoothing**: rms smoothing factor

### Messages

#### Load, save and manipulate modal bank models

- dictionary <dictionary name>
- import
- load
- save
- split
- join
- rename
- delete
- clear
- info
- param
- post
- flush

#### Resonator and other audio parameters

- master
- is_on
- gain
- ampl
- freq
- decay

#### Modes

The resonators can be set to cycle on and off all together or in a number of random patterns

- all_on
- all_off
- cycle
- diffusion
- resonator

#### States

- state
- ramp_to
- ramp_between
- ramp_max
- velocity
- freeze

#### Ranges and selections

- get_ampl_rng
- get_freq_rng
- get_decay_rng
- sel_all
- sel_ampl_ind
- sel_ampl_rng
- sel_freq_ind
- sel_freq_rng
- sel_decay_ind
- sel_decay_rng

### Links

- [CCRMA - Stanford University - Modal synthesis](https://ccrma.stanford.edu/~bilbao/booktop/node14.html)
- [van den Doel & Pai - Modal Synthesis for Vibrating Objects](https://www.cs.ubc.ca/~kvdoel/publications/modalpaper.pdf)

## Author

Yves Candau <ycandau@gmail.com>

## License

This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with this file, you can obtain one at [http://mozilla.org/MPL/2.0/](http://mozilla.org/MPL/2.0/).
