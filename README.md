KKEdit source code editor.

Why another text editor?

Simple, I didn't like any of the others, anyway having used gedit for years it was getting increasingly difficult to keep it compiling without loads of gnome dependencies, looking around linux text editors seem to be VERY simple (leafpad) stupidly complex (GVim,Bluefish), trying to be all things to all people.
One of the best text editors I have used is BBEdit but that is only available on the mac, so I bit the bullet and decided to write my own with the best bits from BBEdit ( the name KKEdit is a tip of the hat to BBEdit ), gedit and leafpad. A simple uncluttered interface, no major dependencies, with just the features I use all the time, and TA DAH! KKEdit.

Some of the features:
Jump to function declaration.
Find Function declaration.
Find and open include file.
Multiple bookmarks.
Run external tools.
Save/Restore session.
Run external tool synchronously or asynchronously.
Pass selected text to external tools.

Find API declarations in installed Gtk-Doc's.

Full source code highlighted printing

+ The usual source view options, line wrap, line numbers etc.

*** NEW ***

Ported to gtk3.

Add --enable-gtk3 to your configure to build for gtk3.

Default is to build for gtk2
You may need to install gtk3 dev files if your distro splits them off from the main package ( debian etc ).

*** NEW ***

gtk-version branch has now been synced with master branch and you shoud now be using the master branch.

*bsd* compile abandoned for the moment as webkit is broken on freeports, can be used with --disable-docviewer.

+ When building from a fesh clone for the first time, please use ./autogen.sh instead of ./configure, args passed to autogen.sh are the same for configure, this only needs to be done on a fresh clone
