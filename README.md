The Rainmeter plugin software development kit contains the necessary tools and examples to build plugins in C/C++ and C#. Note that plugins built with the SDK require Rainmeter 2.3 beta or higher.

**Download:**

*   Use git (`git clone git@github.com:rainmeter/rainmeter-plugin-sdk.git`)
*   Use svn (`svn checkout https://github.com/rainmeter/rainmeter-plugin-sdk/trunk rainmeter-plugin-sdk`)
*   Download the [current snapshot](https://github.com/rainmeter/rainmeter-plugin-sdk/zipball/master) as a .zip archive.

To install the tools required to build the plugins, follow the [instructions](https://raw.github.com/rainmeter/rainmeter/master/Build/Instructions.txt).

**Documentation:** The documentation is available at the [wiki](https://github.com/rainmeter/rainmeter-plugin-sdk/wiki/_pages).

_**Note:**_ To build 64bit C/C++ plugins with Visual C++ Express, right-click on the plugin project(s) -> `Properties` -> `Configuration Properties` -> `General`, and change the `Platform Toolset` field to `Windows7.1SDK` for the Release\x64 and Debug\x64 configurations. With Visual Studio, no changes are necessary.

### Old plugin API

The SDK above uses the "new" plugin API introduced with Rainmeter 2.3 beta and above. The new API, among many other improvements and features, adds support for DynamicVariables. In addition, the new C# API fixes several issues with the old C# API. If your C/C++ plugin is using the old API, we recommend that you consider upgrading to the new API. If your C# plugin is using the old API, we recommend that you upgrade to the new API <b>as soon as possible</b>.</p>