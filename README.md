# Sellke HIV model project.

Here you find the C++ code and Python notebooks for an immuno-epidemiological model of HIV-1. 
The goal of the project is described in the article

> [C.H. van Dorp, M. van Boven and R.J. de Boer](http://journals.plos.org/ploscompbiol/article?id=10.1371/journal.pcbi.1003899),
  Immuno-epidemiological Modeling of HIV-1 Predicts High Heritability of the Set-Point Virus Load, 
  while Selection for CTL Escape Dominates Virulence Evolution,
  PLOS Computational Biology (December 2014)

## COMPILING THE CODE

The program has been created on Linux systems (Ubuntu, CentOS), and probably also compiles on other UNIX systems or even Windows.
For the latter, you will probably have to modify the makefile. The makefile assumes that you have the GNU C compiler
installed (g++). Also make sure that you have the GNU Scientific Library (lgsl, lgslcblas) installed. Then use make to compile the code:

```
$ make
```

from the "hiv-model" directory. This creates the executables "epidemic", "singlehost", and "parametersweep".


## RUNNING THE PROGRAM

Modify parameters in parameters.dat and run the epidemic simulation with e.g.

```
$ ./epidemic -p parameters.dat -s <seed>
```

If all goes well, the program produces an .xml file (name and location specified in the parameter file).
The program `parametersweep` computes the steady state of the model for different parameters. This can be time-consuming.
The python script `parametersweepScript.py` can distribute the workload over multiple CPU cores:

```
$ ./parametersweepScript.py -s <seed> -p parameters.dat -c <number of cores>
```

This will create multiple output files, which are then joined into the desired output file.


## EXAMPLES

The folder `examples` contains a few output files (in the folder `examples/data`).
The ipython notebooks (.ipynb files) can be used for reading and plotting the data.


## MAKING MODIFICATIONS

Please feel free to use and modify the code however you want. The code has been organized (roughly) as follows:
* The source and header files `sellke_class`, `event_class`, `individual_class`, `state_class`, `history_class`, and `statesummary_class`
  define a sort of general event/individual-based algorithm.
* The files `host_class`, `hostlevel_events`, `withinhost_history`, `withinhost_state`, `pathogen_class`, `phase_class`, 
  and `withinhost_statesummary` define exactly what to do for the within-host model.
* The files `epidemic_class`, `poplevel_events`, `poplevel_state_class`, `poplevel_statesummary_class`, `poplevel_history_class`,
  and `host_class` (again) define the behavior of the between-host model.
* `parametersweep_class` and `singlehostsim_class` are used for cohort and/or equilibrium based simulations
* The remaining files contain auxiliary functions and objects.

As a disclaimer, I would like to point out that many of the constructions appeared to be good ideas at the time. Now I would
most likely do things differently. For instance, the `ext_double` object (in `ext_double_class`) extends the real numbers 
with infinity to allow for infinite waiting times. Nice, of course, but also over-complicated and a but confusing. 
The fact that there is an 'abstract' Sellke algorithm forces one to cast pointers back and forth a lot. 
This is dangerous, so be careful!

## REFERENCES

* The algorithm is described fully in our [PLOS Computational Biology article](http://journals.plos.org/ploscompbiol/article?id=10.1371/journal.pcbi.1003899).
  See also references therein.
* For doing some linear algebra, we used the [JAMA and TNT header files](http://math.nist.gov/tnt/).

