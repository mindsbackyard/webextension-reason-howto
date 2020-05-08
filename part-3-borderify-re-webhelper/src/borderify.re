open Belt.Option;
open Webapi.Dom;

let bodyElement =
  document
  ->Document.asHtmlDocument
  ->flatMap(HtmlDocument.body)
  ->flatMap(Element.asHtmlElement)
  ->getExn;

bodyElement->HtmlElement.style
|> CssStyleDeclaration.setProperty("border", "5px solid red", "");