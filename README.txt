$Id$

UUID resolver
=======================================

This module registers menu callbacks to lookup entities with UUID. The default
path for all types of entities is:

  uuid/%uuid

where %uuid is the UUID to search for. Supported site entities for resolution
by UUID include node, node revision, term and user. Initially, only node and
taxonomy term resolution is enabled.

This module can be configured to use different paths for each type of entity
to resolve. The HTTP response code can also be configured. The admin interface
is found under [ Site configuration > Universally Unique IDentifier ] as a tab.