# TStracker
- Version: 1.0.0

## Developing team
- Author: Huan Tran
- Contributor: Harishankar Jayakumar, Tien Dinh

## About
- The goal of TStracker project was to create a recording software that overcomes limits of SpinView software (provided by FLIR along with their Blackfly S camera) and provides better features that more aligned to the tongue-tracking experiment at Kerlin Lab 

## Features
- Simultaneous acquiring from multiple cameras at high speed (up to 1 Khz)
- Guarantee no delay between recording sessions due to data downloading from cameras (User can start recording a new session right after he/she finishes the last one)
- Auto save images to multi-image TIFF files (tiff stack)
- Retrieve and save images' timestamps and imageIDs to the metatags of each image in the tiff stacks
- Provide high multi-image TIFF writing speed with auto tiff stack partitioning feature that overcomes the diminishing-write-speed prblem of LibTiff when operating on multi-image TIFF files
- Timer recording allow user to preset recording duration

## Project structure
```
.
+-- doc 	: this folder contains additional documents to help build the project
+-- lib 	: this folder contains libraries used in this project
+-- src 	: this is where the source code lies
|	+-- DevelopmentSuite 	: This contains example source code use to test a specific features<br/>
|	+-- PythonUtilities 	: Utilities that helps with pleminary tasks like concatinating multipile tiff stack to one file, etc
|	+-- TStracker 		: this is where you should go to and build TStracker
```