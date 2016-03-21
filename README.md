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
+ 

*** NEW ***

Started to port to gtk3, editor compiles and runs and 75% working, use -s switch to kkedit when running to disable plugins, 'cos they don't work yet.

*** NEW ***
