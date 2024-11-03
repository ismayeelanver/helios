<p align="center">
    <img src="./doc/icon.png" align="center" width="30%" height="10%">
</p>
<p align="center">
	<img src="https://img.shields.io/badge/-MIT license-white?logo=opensourceinitiative&logoColor=FFA500" alt="last-commit">
	<img src="https://img.shields.io/badge/-Lightweight-white?logo=sunrise&logoColor=FFA500" alt="last-commit">
	<img src="https://img.shields.io/badge/-Linux Supported-white?logo=linux&logoColor=FFA500" alt="last-commit">
    <br>
	<img src="https://img.shields.io/badge/-Made in C++-white?logo=cplusplus&logoColor=FFA500" alt="last-commit">
</p>
<p align="center"><!-- default option, no dependency badges. -->
</p>
<p align="center">
	<!-- default option, no dependency badges. -->
</p>
<br>

<details><summary>Table of Contents</summary>

- [Overview](#overview)
  - [Key Features](#key-features)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Testing](#testing)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgments](#acknowledgments)

</details>
<hr>

##  Overview

<p>Helios is a minimalist, tiling window manager designed for simplicity, direct control, and high performance. Inspired by the principles of suckless, Helios aims to provide a lightweight environment that prioritizes efficiency and usability for power users and developers alike.</p>

---

### Key Features

- **EWMH Support**: Compliant with the Extended Window Manager Hints (EWMH) specification for better compatibility with modern applications.
- **Lightweight Design**: Built with minimal dependencies, ensuring low resource usage and fast performance.
- **Customizable Configuration**: Easily adjustable settings to tailor the window manager to your preferences.

---

##  Project Structure

```sh
├── config.cpp
├── config.toml
├── helios.cpp
├── include
│   ├── config.hpp
│   ├── helios.hpp
│   ├── key.hpp
│   └── spawn.hpp
├── LICENSE
├── main.cpp
├── meson.build
├── preview.sh
└── README.md
```

---


##  Getting Started

###  Prerequisites

Before getting started with readme-ai, ensure your runtime environment meets the following requirements:

- **Programming Language:** Python
- **Package Manager:** Poetry, Pip, Conda
- **Container Runtime:** Docker


###  Installation

Install readme-ai using one of the following methods:

**Build from source:**

1. Clone the readme-ai repository:
```sh
❯ git clone https://github.com/eli64s/readme-ai
```

2. Navigate to the project directory:
```sh
❯ cd readme-ai
```

3. Build and Install it using the install.sh (**Coming soon**)
```sh
❯ ninja -C build/
❯ bash install.sh
```


###  Testing
Run the test suite using the following command:

```sh
❯ ./preview.sh
```

---

##  Contributing

Join The Discord server or email me <ismayeelanver@gmail.com>

---

##  License

This project is protected under the MIT License.

---

##  Acknowledgments

This project is highly inspired by the dwm of <a href="https://suckless.org">suckless</a>