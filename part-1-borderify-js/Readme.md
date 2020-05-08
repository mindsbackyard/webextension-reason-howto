# Part 1: A basic webextension to build on

To start our journey towards writing webextensions in ReasonML we first create a very simple webextension using ordinary JavaScript.
The webextension which we are creating is [borderify](https://github.com/mdn/webextensions-examples/tree/master/borderify).
The extenstion is used for [Mozilla's introdoctury tutorial](https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/Your_first_WebExtension) on writing webextensions.
The code in this folder is basically a copy of their tutorial.
Please work theirs through before proceeding.

The webextension draws a red border around all web pages on one of Mozilla's subdomains *\*.mozilla.org* (except protected )

## Testing the extension

For testing our extensions we will use [web-ext](https://extensionworkshop.com/documentation/develop/getting-started-with-web-ext/).
`web-ext` is a command line tool which allows us to run a separate Firefox instance with our new addon loaded without affecting our profiles.
The installation instructions are provided in the link above.

To run this example `cd` into the directory containing this `Readme.md` and execute
```
web-ext run
```
Navigate to https://www.mozilla.org in the newly opened browser and you should see a red border around the page.