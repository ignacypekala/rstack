#!/usr/bin/env bash

REPO_URL="https://github.com/ignacypekala/rstack_tests.git"
BRANCH="main"
PREFIX="tests"

git subtree pull --prefix=$PREFIX $REPO_URL $BRANCH --squash
