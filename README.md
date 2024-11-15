<p align="center">
    <img src="./doc/icon.png" align="center" width="30%" height="10%">
</p>
<p align="center">
	<img src="https://img.shields.io/badge/-MIT license-white?logo=opensourceinitiative&logoColor=FFA500" alt="">
	<img src="https://img.shields.io/badge/-Lightweight-white?logo=sunrise&logoColor=FFA500" alt="">
	<img src="https://img.shields.io/badge/-Linux Supported-white?logo=linux&logoColor=FFA500" alt="">
    <br>
	<img src="https://img.shields.io/badge/-Made in C++-white?logo=cplusplus&logoColor=FFA500" alt="">
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
├── config.toml
├── LICENSE
├── meson.build
├── preview.sh
├── README.md
└── src
    ├── config.cpp
    ├── helios.cpp
    ├── include
    │   ├── config.h
    │   ├── helios.h
    │   ├── key.h
    │   └── spawn.h
    └── main.cpp
```

---


##  Getting Started

###  Prerequisites

- **Building enviroment:** ninja


###  Installation

1. Clone the helios repository:
```sh
❯ git clone https://github.com/ismayeelanver/helios
```

2. Navigate to the project directory:
```sh
❯ cd helios
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

This project is protected under the <a href="https://github.com/ismayeelanver/helios/blob/main/LICENSE">MIT license.</a>

---

##  Acknowledgments

This project is highly inspired by the dwm of <a href="https://suckless.org">suckless</a>