# Rasipam: Interactive Pattern Mining of Multivariate Event Sequences in Racket Sports

![](./demo.gif)

#### *This is a GIF version demo. Download the original video [here](https://vis22-demo-video.s3.ap-southeast-1.amazonaws.com/demo.mp4?response-content-disposition=inline&X-Amz-Security-Token=IQoJb3JpZ2luX2VjEDQaDmFwLXNvdXRoZWFzdC0xIkcwRQIgR%2Bxb2FNathxl6mDEayhuoMiSMIqGmdNt527c%2FD%2Bedy8CIQCFtHU1cl4EdJu6haFjI8fWC82slt84oINKQrNn2sWu6CrtAgjN%2F%2F%2F%2F%2F%2F%2F%2F%2F%2F8BEAMaDDUzMjMzNDAzMTY0NCIM4VNjrix%2FcZ9wZRidKsECl0VuX%2BztluVq7VEe1f68ts7lgk8kxoYKmev7q3zFt6jNMGIG4%2FTsBurmPg2FSuQi0XNo1URv3XntriR%2BXzsab%2Bov2d7MLg3q4a2HbAfTzp2vtoYuSs5M0CZWCZ%2B%2BNQcAUw8fVXW%2FMRsFOeC8XI1Eiyethh0h7pzmAq8n0WuRkzvg9Umm%2Fsehcz3opMMBJ%2BBLcwcmrNjmbYAzzz8rfHAMNjCp9iS1xpSaP%2FjDFkbIhinfgmYkOslO5JjWV1w3di87Q%2Fd6bAdmFBWKBMjZQIBTDDkZbArR058a7%2BdnHl%2BxLZbsFTwG3RpUNATh5CDW4z5bz9uWH4hsUOp4%2Fgxq%2FUNrMjmzALmYOM1wV%2BN8UkXxgvT%2B6z%2BYr5iVxBvzaNOwqfzLMwKTPQqD9C7dXcCFFs1xuTs81zzMuhAWYcxuLpIEqC9KMNrWvpIGOrMC%2BLhbKsmvTK0DKj3NUkPplxpMKR2whm5wPqp6n4jZrfgVKm6PKFdHx6IYVRWk1pEqTz1d3LnkDKHAX1BIOCBHemqfjriKvO6jYk6rnm6x3s1ELFcr98D00pa7tV7DiaPMH%2FIagrb8Vy22nmbvfnO0lRSHja9SWxjJCkU4AbSxSo%2BZ%2FupSaXBnBZyyidgqdcANR%2B276PJ3t%2BDAmZ1%2B4zfF7sEL8ZRBoAhqHGoOH5HfRp%2FUL6rfXBywA5i1ZVKPorgrTKaJ5LSY%2BKJbvCkuIq3WN0OxfY2bXENWUaA%2BOP5JLj8eUkl75vwPiJuffLWU6Yrjk5imM3Pzm89OjifWnyL8Zd6%2FL9JyhxXv1U79eCXfXsLgykng4EQ7KBL5Suk1bBZOSCpH%2FB94p0lnsmziyVhmokzLFg%3D%3D&X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Date=20220408T033210Z&X-Amz-SignedHeaders=host&X-Amz-Expires=300&X-Amz-Credential=ASIAXX4MUWMOE5TRWNPF%2F20220408%2Fap-southeast-1%2Fs3%2Faws4_request&X-Amz-Signature=6a6fcd5dddeab6d88834795449b4aa5d3c80b0713aedaf6ab6f5021ac2157eda) (1080P，with audio).*

## Overview

This is the code accompanying the paper for the IEEE VIS 2022 submission, *Rasipam: Interactive Pattern Mining of Multivariate Event Sequences in Racket Sports*

The project is split into three parts: algorithm, backend, and frontend. Algorithm is writen in C++ and can be installed as a Python package. Backend is writen in Python and built on [FastAPI](https://fastapi.tiangolo.com/). Frontend is writen in JavaScript [React](https://reactjs.org/).

The following software is needed to run the code:

- CMake with a C++20 compiler
- Python
- JavaScript
- yarn

## Algorithm

**Install.** 

```shell
cd algorithm
python setup.py install
```

**Quantitative experiments.** We provide a python scirpt `quantitative_data_generator.py` for quantitative data generating and a bash script `quantitative_test.sh` for automatically running the test pipeline.

```shell
cd algorithm
cmake .
bash quantitative_test.sh <PATH_TO_THE_EXECUTABLE_FILE_PRODUCED_BY_CMAKE> <PATH_TO_QUANTITATIVE_DATA_GENERATOR>
```

## Backend

**Prerequirements.** Install the package and its dependencies:

```shell
pip install -r requirements.txt
```

**Run the backend.** We use uvicorn as web server for backend. Run `pip install uvicorn` to install uvicorn if necessary.

```shell
uvicorn api:app --reload
```

## Frontend

We use `yarn` as the package manager of frontend (another alternative is `npm`).

**Install `node_module`.**

```shell
yarn install
```

**Run the frontend.**

```shell
yarn start
```

