# 🥡 張る Haru

## 🥡 CMake project generator for C and C++

`haru` helps you initialise a barebones CMake project.

The initialisation process can be customised
through prompts during project initialisation.

Options available are further detailed [below](#-usage).

Alternatively leave everything to the default values
for a sensible basic project setup.

___

## 📌 Features

- [X] Generate ready-to-go CMakelists.txt
- [X] Support both C and C++ (or combined) projects
- [X] Define standard library version for each language
- [X] Include default formatting and git configuration if desired
- [X] Include default dependencies if desired
- [X] Add dependencies at project creation time
- [X] Add dependencies at project creation time
- [X] Initialise git repository after project creation
- [X] Initialise CMake after project creation

___

## 💻 Usage

Main two commands to get a new project started are:
|  Command  |                   Description                       |
|---------- | --------------------------------------------------- |
| `create`  | Create a new directory with the name of the project |
| `init`    | Initialise a project in the current directory       |

Both commands allow using default values
by issuing the flag `-d` or `--use-defaults`.

Using defaults skip the configuration section,
with the exception of having to enter the project name when using `create`.

For `create`, project creation will be aborted by default
if a directory already exists with the project name.

You can allow overwriting the existing directory with `-f` or `--force`.

After this `haru` will ask you about project settings:
<!-- markdownlint-disable MD013 -->
|     Option           |                       Description                                    |
| -------------------- | -------------------------------------------------------------------- |
| Name                 | Name for the project. If run with `init` will default to PWD         |
| CMake version        | Minimum CMake version. Defaults to 3.28                              |
| Version              | Project version string. Defaults to 0.1.0                            |
| Languages            | Languages to use for project. Allows C, C++ or both. Defaults to C++ |
| Standard             | Standard library editions. Defaults to C++23 and C17                 |
| Entrypoint           | Entrypoint for the project. Defaults to `src/main.{c\|cpp}`          |
| Default files        | Default files to include, .clang-format etc. Defaults to all         |
| Default dependencies | Whether to add default dependencies. Defaults to yes                 |
| Dependencies         | Add arbitary dependencies if desired.                                |
<!-- markdownlint-enable MD013 -->

Second function for `haru` is initialising an existing CMake project.

Running `haru cmake init` will try to initialise the project according
to a `haru.conf` file located at the project root.

If this local configuration file does not contain a value required for
initialising the project, it will first look at your global `haru.conf` file
located at `$HOME/.config/haru.conf`.

When no configuration is found, a default value will be used instead.

It's possible to override the local configuration
with your own global values by using the `-g` or `--global` flag.

___

## 📋 Requirements

### 💻 Using

For general usage, `haru` is completely self contained in a single binary.
However, a few external tools are used - when present -
to clean up the generated output.

- `gersemi`
  - Runs when `.gersemirc` is included in project initialisation.
  - Formats all generated CMake files during project setup.
- `clang-format`
  - Runs when `.clang-format` is included in project initialisation.
  - Formats generated entry point source file.
- `git`
  - Optionally initialise a git repository after generation
- `cmake`
  - Optionally initialise the CMake project after generation

### 🛠 Building

For building requirements are:

- C++23 compatible compiler. `clang >= 17.0` recommended.
- `cmake` >= `3.27`
- `xxd` is needed to generate `.cpp` files from the templates in [assets/](assets/).
  - Requires support for C-style output
  - On Windows use <https://github.com/ckormanyos/xxd>.
