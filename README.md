# Real Engine <a href='http://5.9.149.113:8082/job/RealEngine/'><img src='http://5.9.149.113:8082/buildStatus/icon?job=RealEngine' align="right"></a>

Welcome to the source code of the RealEngine, an engine written in C,
with minimal dependencies.

This repository contains everything you need for compiling the engine, and start
tinkering with it. We're excited to see what you can create with it!

Read the [documentation].

If you need help, just ask, you can reach me at ag@arvid.io or at the [forums].


# Getting started

Currently supported operating systems:

- Windows


To compile the engine, just calling FASTBuild is enough:

    $ ./extras/bin/win64/fbuild.exe


FASTBuild has the following targets:

- `all`: builds everything, game, engine, resources, tests, etc
  - `tools`: build all tools
      - `lolpack`: build lolpack
      - `objbin`: build objbin
  - `game`: builds the game and engine
    - `libc`: builds the libc static library
    - `engine`: builds the engine static library
    - `game-dll`: builds the game dll
    - `game-exe`: build the game exe
    - `crash`: builds the gerr.exe, to catch and send crash reports
  - `server`: builds the game server
    - `server-dll`: builds the server dll
    - `server-exe`: builds the server exe
- `clean`: remove all build artifacts
- `dist-clean`: remove all build artifacts and install artifacts

It works perfectly fine to always just build `all`, it's faster and preferred
to build only the target you currently require to be updated, since `res` might
take a while to build and you don't want them to be rebuild regularly.

The typical workflow consists of building the resources once and than only
rebuilding the `game` or `server` target if source updates are made.

If enabled, the DLLs will hotreload after `game` or `server` was build.


# Visual Studio

The `vs` FASTBuild target will build a Visual Studio solution which can be opened
and used for development.

Keep in mind, while the project is opened in Visual Studio, it's still build
by FASTBuild.


# Branches

The `release` branch points to the latest stable release, which has extensively
gone through QA. A new released is roughly published every few months.
Provides a great starting point to learn the engine.

The `stable` branch is tried to be kept in a stable state at all times, but will
contain fixes and features from master considered stable. This provides a great
balance between getting the latest features and having a stable build.

The `master` branch is used for development, and might not even compile. All
pull requests must be filed against it.
**Should not be used for production!**

Other branches (normally named `feature/<something>`) may be present. <br>
They're used for bigger individual feature development, and can safely be ignored.


# Development

Please read the [CONTRIBUTING].


# Changelog

Read the [CHANGELOG on master] for latest bleeding edge changes.

Read the [CHANGELOG on stable] for latest stable changes.

Read the [CHANGELOG on release] for latest release changes.


# License

Copyright (C) 2016 Arvid Gerstmann

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; **Only licensed under version 2, not
any later version.**

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


[documentation]:            https://arvid.io
[forums]:                   https://arvid.io
[CONTRIBUTING]:          https://github.com/Leandros/RealEngine/blob/master/CONTRIBUTING.md#
[CHANGELOG on master]:   https://github.com/Leandros/RealEngine/blob/master/CHANGELOG.md#
[CHANGELOG on stable]:   https://github.com/Leandros/RealEngine/blob/stable/CHANGELOG.md#
[CHANGELOG on release]:  https://github.com/Leandros/RealEngine/blob/release/CHANGELOG.md#
