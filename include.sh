#!/usr/bin/env bash

## GETS THE CURRENT MODULE ROOT DIRECTORY
PTR_TEMPLATE_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

source $PTR_TEMPLATE_ROOT"/conf/conf.sh.dist"

if [ -f $PTR_TEMPLATE_ROOT"/conf/conf.sh" ]; then
    source $PTR_TEMPLATE_ROOT"/conf/conf.sh"
fi
