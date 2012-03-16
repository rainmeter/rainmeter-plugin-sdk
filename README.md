The Rainmeter plugin software development kit contains the necessary tools and examples to build plugins in C/C++ and C#. Note that plugins built with the SDK require Rainmeter 2.3 beta or higher.

**Download:**

*   Use git (`git clone git@github.com:rainmeter/rainmeter-plugin-sdk.git`)
*   Use svn (`svn checkout https://github.com/rainmeter/rainmeter-plugin-sdk/trunk rainmeter`)
*   Download the [current snapshot](https://github.com/rainmeter/rainmeter-plugin-sdk/zipball/master) as a .zip archive.

To install the tools required to build the plugins, follow the "Build tools" section of the <a href="http://rainmeter.net/cms/Source">Source Code</a> page.

**Documentation:** The documentation is available at the [wiki](https://github.com/rainmeter/rainmeter-plugin-sdk/wiki/_pages).

_**Note:**_ To build 64bit C/C++ plugins with Visual C++ Express, right-click on the plugin project(s) -> `Properties` -> `Configuration Properties` -> `General`, and change the `Platform Toolset` field to `Windows7.1SDK` for the Release\x64 and Debug\x64 configurations. With Visual Studio, no changes are necessary.


### Plugin SDK Changelog

#### Feb 22, 2012

*    Added support for ExecuteBang.

#### Feb 5, 2012

*    Initial release.

### Old plugin API

The SDK above uses the "new" plugin API introduced with Rainmeter 2.3 beta and above. The new API, among many other improvements and features, adds support for DynamicVariables. In addition, the new C# API fixes several issues with the old C# API. If your C/C++ plugin is using the old API, we recommend that you consider upgrading to the new API. If your C# plugin is using the old API, we recommend that you upgrade to the new API <b>as soon as possible</b>.</p>