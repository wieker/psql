Simple, slow and very stupid implementation of widget system.
Each widget derived from Gtk widget. Widget can be Gtk widget
or can contain Gtk or high level widgets. Each widget has
it container. High level widgets are containers for sublevels.

Each widget has size and position.
For first stage position is relative for it containter. Each
position can be calculated. Position can be translated into
percent of containers width.

First version of rendering alg (very slow and stupid):
- parse UI file
- calculate absolute coordinates of each element.
- create widgets and set coordinates
