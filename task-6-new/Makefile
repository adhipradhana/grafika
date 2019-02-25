#!/bin/bash

export NOW=$(shell date)

build: 
	@echo "${NOW} === BUILDING MAIN ==="
	@echo
	@g++ main.cpp draw.cpp primitive.cpp -o main -lm
	

run: build
	@echo "${NOW}: Running Main from example_scene.txt ..."
	@sudo ./main example_scene.txt
