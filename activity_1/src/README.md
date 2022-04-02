# Problem Implementation using Gurobi (in Python)

## Requirements

- [texlive](https://tug.org/texlive/)
- [latexmk](https://mg.readthedocs.io/latexmk.html)
- [gurobi](https://www.gurobi.com) 9.5
- [python](https://www.python.org/) 3.7.12
- [conda](https://docs.conda.io/en/latest/miniconda.html) 4.11.0

## Runnning the code

### Install dependencies

For most dependencies, one can create a conda environment:

```sh
conda env create -f environment.yml
```

Notice, however, that the gurobi dependency must be installed manually.

### Python files

The figures in the report were generated with [`generateFigures.py`](./generateFigures.py).

The results provided in the report can be reproduced with [`testTimeExecution.py`](./testTimeExecution.py).

The other files implement the mothods required for generating the instances and solving the Linear Programming problem.
