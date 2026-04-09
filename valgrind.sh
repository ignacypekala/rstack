#!/bin/bash
valgrind --track-origins=yes --leak-check=full --error-exitcode="3" -q ./$*
