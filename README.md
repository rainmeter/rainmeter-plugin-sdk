The Rainmeter plugin software development kit contains the necessary tools and examples to build plugins in C/C++ and C#. Note that plugins built with the SDK require Rainmeter 4.5 or higher.

**Download:**

*   Use git (`git clone git@github.com:rainmeter/rainmeter-plugin-sdk.git`)
*   Download the [current snapshot](https://github.com/rainmeter/rainmeter-plugin-sdk/zipball/master) as a .zip archive.

The SDK can be built using Visual Studio 2022 or later (Visual Studio 2026 included). The C++ projects use whichever platform toolset the installed Visual Studio provides by default (v143, v144 or v145), so no project changes are needed when moving between versions. To force a specific toolset, build with `msbuild /p:PlatformToolset=v143` or change it in the project properties. If you don't already have Visual Studio, you can download the free "Community" version [here](https://www.visualstudio.com/downloads/).

**Documentation:** The documentation is available at [docs.rainmeter.net/developers](https://docs.rainmeter.net/developers/)
