# Rasipam: Interactive Pattern Mining of Multivariate Event Sequences in Racket Sports

https://user-images.githubusercontent.com/6877923/115474571-03c75800-a23e-11eb-8096-8973aad5fa9f.mp4

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

