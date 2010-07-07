# mkd-wiki

## WTF is it ?

`mkd-wiki` is a simple wiki system. Basically, you write stuff in your favorite
text editor, and typing `make` will build a static website containing this
stuff. "Stuff" can contain
[Markdown](http://daringfireball.net/projects/markdown/) formatting, including
internal and external links.

## Prerequisites

You will need :

  * [pandoc](http://github.com/jgm/pandoc), a document formatter
  * [perl](http://www.perl.org/), the Pathologically Eclectic Rubbish Lister
  * (optional)
    [hsgtd](http://www.mlesniak.com/haskell/gettings-things-done-in-haskell/),
    a great Get-Things-Done program
  * (optional) [pal](http://palcal.sourceforge.net/), a calendar program

## Howto

The wiki is pre-filled with a single 'how-to' page. To see it, just type make
and browse to the `html/index` file.

## Plugins

(well, sort of) The makefile recognize special target names, as listed in
`mk/special.mk`. If one of your pages contain a link to 'calendar', a static
view or your calendar will be built. Same for 'gtd' and your TODO-list.

## Contributing

I hacked this small wiki 'app' to suit my needs. If you think it can be
improved, feel free to send me some patches !
