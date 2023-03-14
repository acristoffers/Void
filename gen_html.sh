#!/bin/bash

touch html
rm -r html
mkdir html

pushd WelcomeScreen
yarn install
yarn run ng build $1
popd
cp -r WelcomeScreen/dist/WelcomeScreen html/WelcomeScreen

pushd StoreScreen
yarn install
yarn run ng build $1
popd
cp -r StoreScreen/dist/StoreScreen html/StoreScreen

python3 gen_resource.py
