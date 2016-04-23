#!/usr/bin/env bash

coffee -c html/js/*.coffee
scss -t compressed --update html/css
rm -r .sass-cache
