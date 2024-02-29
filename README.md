# 🥡 張る Haru

## 🥡 CMake project generator for C and C++

`haru` can be used to initialise a barebones CMake project.  
This initialisation process can be customised
by responding to prompts during project initialisation.  
These options are further detailed [below](#-usage).  
Alternatively leave everything to the default values,
which strive to provide a sensible basic project setup.  

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

Both commands allow using all default values
by issuing the flag `-d` or `--use-defaults`.  
This will skip the configuration section,
aside from having to enter the project name when using `create`.  

For `create`, if a directory already exists with the project name,
the project creation will abort by default.  
This behaviour can however be overridden with the `-f` or `--force` flag
to allow overwriting the directory when found.  

After this `haru` will ask you for input about project settings:
<!-- markdownlint-disable MD013 -->
|     Option           |                       Description                                    |
| -------------------- | -------------------------------------------------------------------- |
| Name                 | Name for the project. If run with `init` will default to PWD         |
| CMake version        | Minimum CMake version. Defaults to 3.28                              |
| Version              | Project version string. Defaults to 0.1.0                            |
| Languages            | Languages to use for project. Allows C, C++ or both. Defaults to C++ |
| Standard             | Standard library editions. Defaults to C++23 and C11                 |
| Entrypoint           | Entrypoint for the project. Defaults to `src/main.{c\|cpp}`          |
| Default files        | Default files to include, .clang-format etc. Defaults to all         |
| Default dependencies | Whether to add default dependencies. Defaults to yes                 |
| Dependencies         | Add arbitary dependencies if desired.                                |
<!-- markdownlint-enable MD013 -->

___

## 📋 Requirements

### 💻 Using

For general usage, `haru` is completely self contained in a single binary.  
However, a few external tools are used, when present,
to make the generated output better.

- `gersemi`
  - Runs when `.gersemirc` is included in project initialisation.
  - Formats all generated CMake files during project setup.

### 🛠 Building

For building requirements are:

- C++23 compatible compiler. `clang >= 17.0` recommended.
- `cmake` >= `3.27`
- `xxd` is needed to generate `.cpp` files from the templates in [assets/](assets/).
  - On Windows use <https://github.com/ckormanyos/xxd>.
