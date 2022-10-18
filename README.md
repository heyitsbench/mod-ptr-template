# PTR Character Templates

- Latest AzerothCore build status as of the [latest commit](https://github.com/heyitsbench/mod-ptr-template/commit): [![core-build](https://github.com/heyitsbench/mod-ptr-template/actions/workflows/core-build.yml/badge.svg)](https://github.com/heyitsbench/mod-ptr-template/actions/workflows/core-build.yml)

This is a module for [AzerothCore](http://www.azerothcore.org/) that adds commands for creating characters from preset templates.

## Important Notes

This module depends on the selection of certain template sets that have already been filled out. The sets included in this module (WIP as of the writing of this readme) are based on the template characters as seen on the Classic and Season of Mastery public test realms. In the future, this may be expanded to include many more sets, and anyone willing is free to contribute to this module.

Check issues page for current progress on blizzlike templates.

## How to Install

For the most part, the [guide found here](https://www.azerothcore.org/wiki/add-a-module) can be referred to. However, the SQL portion needs a bit of attention.

If you want to create your own template sets, feel free to only run the [structure.sql](https://github.com/heyitsbench/mod-ptr-template/blob/master/data/sql/db-world/structure.sql) file in your world DB and create the sets from the created tables.

If you want to use the template sets included with the module, you must then run the contents of the [blizzlike](https://github.com/heyitsbench/mod-ptr-template/tree/master/data/sql/blizzlike) directory on your world DB as well.

### (Optional) Edit Module Configuration

Though currently sparse, you are also able to edit the [mod_ptr_template.conf](https://github.com/heyitsbench/mod-ptr-template/blob/master/conf/mod_ptr_template.conf.dist) to your liking.

## Creating Template Sets

For information on creating your own template sets (and submitting them here for others to use), please refer to the [wiki](https://github.com/heyitsbench/mod-ptr-template/wiki).

## Credits
- [acidmanifesto](https://github.com/acidmanifesto) for providing an example of storing SQL results into variables.
- [AnchyDev](https://github.com/AnchyDev) for pointing out some much needed info regarding item storage.
- [Nyeriah](https://github.com/Nyeriah) for identifying a couple points of confusion to me, resulting in issues.
