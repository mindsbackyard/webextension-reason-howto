# Part 2: Enabling ReasonML in our webextension

As a first step to *"reasonify"* our webextension we try to write the same extension using Reason's support for embedding raw JS in Reason files.
This simple change let's us focus on compiling our Reason extension into JS and making it available to our webextension.

## Setting up ReasonML

Let's create a new directory to hold our Reason extension from the command line.
```bash
bsb -init borderify-reason -theme basic-reason
```
The command is straight out of the [Reason documentation](https://reasonml.github.io/docs/en/installation#new-project).
It sets up a `bsconfig.json` defining our configuration for Reason/BuckleScript, and a `package.json` to manage our dependencies via npm/yarn.

The *theme* we used for setting up our project createad a `src/` directory where we will place our Reason files, but first clean it up.

## Creating our Reason webextension

Now on to the actual webextension code.
We create a `src/borderify.re` and fill it with the following content:
```reason
[%%bs.raw {| document.body.style.border = "5px solid red" |}];
```
The line above embeds plain JS into our Reason code.
You can read more on this in the Reason docs on [JS interop](https://reasonml.github.io/docs/en/interop) and the BuckleScript docs on [embedding raw JS](https://bucklescript.github.io/docs/en/embed-raw-javascript).
Remember that Reason is just a different syntax for OCaml which is transpiled into JS via [BuckleScript](https://bucklescript.github.io/en/).

To compile our Reason code execute:
```bash
bsb -make-world
```
or use the scripts in our `package.json` which wehre automatically defined during project generation, e.g., via `yarn`:
```bash
yarn build
```
Note that whenever we change our Reason code we need to build our project again.
Though this can also be done automatically.
To learn more about the build system read on in the BuckleScript docs on [bsb](https://bucklescript.github.io/docs/en/build-overview).

Finally, we need to teach our webextension where to find our transpiled JS code.
The way we set up the build each `.re` file in `src/` is transpiled into a corresponding `.bs.js` file.

Let's copy over our `manifest.json` from the first part of the guide.
The last thing we need to do is to swap out our content script in the `manifest.json` to:
```json
...
"content_scripts": [
    {
        "matches": [
            "*://*.mozilla.org/*"
        ],
        "js": [
            "src/borderify.bs.js"
        ]
    }
]
...
```

Now we can test our webextension via `web-ext run` as in part 1.