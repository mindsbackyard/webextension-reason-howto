# Part 3: Using actual ReasonML in our webextension

Now it is time to write some actual Reason and remove the unsafe JS code.
Start with copying over the final state of part 2.

To add the red border to our web page via Reason we need some way to access the page's DOM from Reason.
We need an external library for that which wraps the JS DOM API for Reason.
Luckily, someone else has already taken on that task.

So let's head over to [Redex](https://redex.github.io) (Reason Package Index) and naively search for `DOM`.
Among the first hits at the time of writing we find [bs-webapi](https://redex.github.io/package/bs-webapi) a library which makes the browser's DOM API available for Reason.
We are going use this library now in our project:

1. Add the library to our project via yarn/npm:
    ```
    yarn add bs-webapi
    ```
2. Add `bs-webapi` to our `bs-dependencies` in `bsconfig.json`:
    ```json
    {
        ...
        "bs-dependencies": [
            "bs-webapi
        ]
    }
    ```

## Interlude: JS wrappers & external dependencies

Let's observe a few things here before we continue.
When we add the `bs-webapi` dependency via yarn/npm the package is added to our project's `package.json` similar to this:
```json
{
    ...
    "dependencies": {
        "bs-webapi": "^0.15.9"
    },
    ...
}
```
This deals with JS-side of things and makes JS module loader, e.g., `commonjs` aware of the external dependency.
Therefore, once our Reason code is transpiled to JS the JS runtime with the help of the module loader will be able to use the external dependency we just defined.

Though if we look into the [source code](https://github.com/reasonml-community/bs-webapi-incubator) of `bs-webapi` (*you can also use the local version in your projects `node_modules/` directory*) we see that it mostly consists of `external` declarations.
[External]() is the mechanism for Reason & BuckleScript to refert to native or JS libraries.
You can read more on interoperability in [Reason's interop](https://reasonml.github.io/docs/en/interop) and [BuckleScript's interopt](https://bucklescript.github.io/docs/en/interop-overview) chapters (*BuckleScript's docs are much more extensive here*).

One thing is special though: Usually the Reason wrapper would refer to the wrapped JS package in its own `package.json`.
If we check `bs-webapi`'s [package.json]() we won't find any further dependencies.

**Q:** Where else does the library code doing the heavy-lifting come from then?

**A:** It is already part of our browser.
Our transpiled JS code for our webextension is later executed by the browser's JS runtime which provides the implemenation of the DOM.

The entry in `bsconfig.json` then takes care that `bsb` is able to pick up the module/function/type declarations of `bs-webapi`.

## Using bs-webapi (I)

Clean out `boderify.re`.
We start with making the page's `document` object available.
It is declared in the `Webapi.Dom` module so we open it up and additionally we will also open `Belt.Option` because we will deal with several `option('a)` types going forward:
```re
open Belt.Option;
open Webapi.Dom;
```

## Interlude: Belt, bs-webapi, and types
If you are new to Reason/BuckleScript/OCaml here a few pointers:

* `Belt` is *a* [standard library for BuckleScript](https://bucklescript.github.io/docs/en/stdlib-overview) and it already ships with it---so no need to install.
   It is different from OCaml's standard library which is also shipped with BuckleScript---with BuckleScript being an implementation of the [OCaml language](https://ocaml.org/).
   OCaml's standard library is quite minimalistic on purpose and has different design goals.
   OCaml's approach is to rely on additional standard libraries, `Belt` being only an example.
   For example `Base` is a different standard library commonly used with OCaml.

* When reading library code, e.g., for `bs-webapi` or `Belt` as it originates from BuckleScript many use standard OCaml syntax instead of Reason syntax.
  Both are equivalent in semantics, they are just written differently.
  As an example when reading types with type arguments (aka generics) in Reason we see
  ```re
  type mytype('a)
  ```
  whereas in OCaml the same type would be written as
  ```ocaml
  type 'a mytype
  ```

* `bs-webapi` makes intensive use of generic types, though many are not defined in the library itself but especially the [DOM types](https://bucklescript.github.io/bucklescript/api/Dom.html) are shipped with BuckleScript.
   The DOM types use a kind of subtyping using chained types with type variables.
   An example are the `document` and `htmlDocument` types---here in Reason syntax instead of OCaml as in the actual module:
   ```re
   type _baseClass  // this type is used to mark the end of a particular "type chain"
   ...
   type _document('a)  // used to mark a type 'a as a "document"
   type document_like('a) = node_like(_document('a))  // any concrete document will be an instantiation of this type
   type document = document_like(_baseClass)  // the type representing a browser's document object
   type _htmlDocument  // this is a type used to mark the end of another type chain
   type htmlDocument = document_like(_htmlDocument)  // the type representing a browser's html document
   ```
   First, why `document` and `htmlDocument`?
   In [JS' web API](https://developer.mozilla.org/en-US/docs/Web/API/HTMLDocument) the `HtmlDocument` interface inherits from the `Document` interface.
   Paraphrasing the Mozilla docs: The difference between the two is that a `HtmlDocument` is created from a HTTP server response with a `Content-Type: application/html` header set, whereas `Document` does not have this restriction.

   Second, why all those types?
   They basically form a kind of type hierachy to allow to implement functions to accept multiple types.
   Instead of defining a function to accept a `document` and another one to accept a `htmlDocument` we can define it on `document_like('a)` if it should work for all kinds of documents, or on `node_like('a)` if it should work for other DOM nodes too.

* `bs-webapi` also make heavy us of functors to instantiate modules for specific types.
   For example the module `Webapi.Dom.Document` includes all functions defined on nodes specialised for `Dom.document` by using the `Webapi.Dom.Node.Impl` functor.
   `Webapi.Dom.Node` defines the same subset of functions but specialises them for `Dom.node`.
   One could have maybe used the subtype relationship from the previous point but by a [design decision](https://github.com/reasonml-community/bs-webapi-incubator/pull/23) it has not been done.

## Using bs-webapi (I)
As we want to set the CSS style on our page's `body` element we need to access it first:
```re
let bodyElement =
  document
  ->Document.asHtmlDocument
  ->flatMap(HtmlDocument.body)
  ->flatMap(Element.asHtmlElement)
  ->getExn;
```
Seems a bit complicated?
What are we doing here?

1. Browsing through the `bs-webapi` we see that the `body` element is only defined on `HtmlDocuments`.
   `document` itself is of type `Dom.document` though as this is the only interface guaranteed by the browser.
   So we need to *try* to convert it into a `HtmlDocument`.
   It is basically like downcasting in a inheritance hierarchy which is always inherently unsafe.
   We use `document->Document.asHtmlDocument` to do that which returns a `option(htmlDocument)` because the "cast" can fail if the underlying JS `document` object is not a `HtmlDocument`.
2. We want to extract the body element, though we only want to do that if the conversion succeeded.
   Therefore we need to `flatMap` our `option` which applies the `HtmlDocument.body` function---note it's function here not a field of a record---if and only if the `option`s variant is `Some`.
   As `body` can itself fail and therefore returns an `option` too we are using `flatMap` instead of `map` to remove one layer of nesting `option` types---going forward we don't care whether the document conversion failed or it succeeded and the body extraction faield.
3. To set CSS styles we need a `Dom.htmlElement` instead of a `Dom.element` as returned by `body`.
   So try to convert it too.
4. If the previous steps succeeded we extract the body HTMLElement or raise an exception otherwise.

Finally, we want to set a property of the body elements `style` attribute so we call
```re
bodyElement->HtmlElement.style
|> CssStyleDeclaration.setProperty("border", "5px solid red", "");
```
The last part is interesting again as we are using the "pipe-last" `|>` operator instead of the "pipe-first" `->` operator.
This is due to the type signature of `CssStyleDeclaration.setProperty` which expects the style value as its last argument instead of its first.

Done.
Let's try it out again using `web-ext` and accesss https://www.mozilla.org.
Though, something doesn't seem to work out---there's no red border around the page.

To figure out what went wrong we neeed additional debug output.
Let's run
```
web-ext run --verbose
```
to gather it.
We'll see a log line in our console similiar to:
```
[firefox/index.js][debug] Firefox stderr: JavaScript error: moz-extension://0cb33569-e273-485d-9299-a239d5c3a990/src/borderify.bs.js, line 4: ReferenceError: require is not defined
```

What happened?
When we inspect the transpiled `borderify.bs.js` we see that our `open` instructions have been translated to to `require` calls .
Unfortunately, the context in which the browser runs our webextensions does not provide a `require` implementation which is usually provided by a module loader like `common-js` or `AMD`.
Further, also our project's dependencies are not made available to the webextension.

## Packing ReasonML webextensions

Next, we will make our dependencies and a module loade available to our webextension.
The loader which we will use is [webpack](https://webpack.js.org/)---although different loader can be used if you know how to configure it.
Webpack will take care of gathering our transpiled JS source files, the project's dependencies, and a module loader implementation and pack it into single JS file which we will then use as our webextension's content script.

We need to install `webpack` and its command line interface
```
yarn add --dev webpack webpack-cli
```
and configure `webpack` to produce a JS file which we want to pass to our webextension.
We create a `webpack.config.js` in our project root with the following content
```js
const path = require('path');

module.exports = {
    entry: './src/borderify.bs.js',
    output: {
        filename: 'borderify.packed.js',
        path: path.resolve(__dirname, 'dist'),
    },
    mode: 'production'
};
```
to produce packed JS file in `dist/borderify.bs.js`.
Further, we need to reconfigure our `manifest.json` to point the webextension to use the packed file as a content script:
```json
{
    ...
    "content_scripts": [
        {
            ...
            "js": [
                "dist/borderify.packed.js"
            ]
            ...
        }
    ]
    ...
}
```
Now, let's run `webpack` to create our bundle.
As we installed `webpack` using npm/yarn we need to use `npx` to execute the NodeJS executable:
```
npx webpack
```

If everything went well a `dist/borderify.packed.js` should be created.
If we now try to rerun our webextension via `web-ext` and load `https://www.mozilla.org` we should finally see a red border!
We have finished our first webextension using actual Reason code!


## Interlude: Automating the build

In our current setup every change to our source files requires us to run two commands to update our webextension:
```sh
yarn build
npx webpack
```

Ideally we want to limit this to a single command to kick off our build process.
We could create shell script or Makefile to chain the commands but we are already using three layers of build systems: `yarn`, `bsb`, and `webpack`.
There shouldn't be a need to add another one.

*To be continued ...*