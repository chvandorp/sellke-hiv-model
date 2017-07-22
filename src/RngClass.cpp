/*
 * RngClass.cpp
 *
 *  Created on: Aug 4, 2014
 *      Author: chris
 */

#include "RngClass.hpp"

RngClass RNG;


/******* members for  RngClass ********/

RngClass::RngClass() {
	init(); // uses standard seed
}
RngClass::RngClass(unsigned long s) {
	init();	seed(s);
}
RngClass::~RngClass() {
	gsl_rng_free(gslRng);
}
void RngClass::init() {
	gslRng = gsl_rng_alloc(gsl_rng_taus);
}
void RngClass::seed(unsigned long s) {
	gsl_rng_set(gslRng,s);
}

// distributions

unsigned long RngClass::Integer() {
	return gsl_rng_get(gslRng) - gsl_rng_min(gslRng);
}
unsigned long RngClass::Integer(unsigned long max) { // modulo max
	return gsl_rng_uniform_int(gslRng, max);
}
double RngClass::Uniform() { // uniform(0,1), excludes 0 and 1
	 return gsl_ran_flat(gslRng, 0.0, 1.0);
}

double RngClass::Uniform(double a, double b) { // Uniform(a,b)
	 if ( a <= b ) {
		 if ( a==b ) {
			 return a;
		 }
		 else {
			 return gsl_ran_flat(gslRng, a, b);
		 }
	 }
	 else {
		 throw ex_rngError();
	 }
}

double RngClass::Normal(double mu, double sigma) { // mean and sd
	if ( sigma >= 0.0 ) {
		if ( sigma == 0.0 ) {
			return mu;
		}
		else {
			return mu + gsl_ran_gaussian(gslRng, sigma);
		}
	}
	else {
		throw ex_rngError();
	}
}

double RngClass::SkewNormal(double m, double s, double alpha) {
	if ( s < 0.0 ) throw ex_rngError();
	if ( s == 0.0 ) return m; // in the limit s --> 0
	// now handle non-trivial cases
	double Z1 = Normal(0.0,1.0);
	if ( alpha == 0.0 ) return s * Z1 + m; // not skewed, just normal
	else {
		// X = 1/sqrt{1+alpha^2} alpha |Z1| + Z2
		double Z2 = Normal(0.0,1.0);
		double absZ1 = (Z1 > 0 ? Z1 : -Z1);
		return m + s * ( (alpha*absZ1 + Z2)/sqrt(1.0 + alpha*alpha)  );
	}
}

double RngClass::Exponential(double lambda) {
	if ( lambda > 0 ) {
		return gsl_ran_exponential(gslRng, 1.0/lambda);
	}
	else {
		throw ex_rngError();
	}
}

double RngClass::Weibull(double a, double b) { // scale, shape
	if ( a >= 0.0 && b >= 0.0 ) {
		if ( a == 0 ) {
			return 0.0;
		}
		else {
			return gsl_ran_weibull(gslRng, a, b);
		}
	}
	else {
		throw ex_rngError();
	}
}

double RngClass::Erlang(double lambda, unsigned k) { // simple Erlang(rate,shape) (sum of exponentials)
	if ( lambda > 0 ) {
		double ans = 0.0;
		for ( unsigned i = 0; i < k; ++i ) {
			ans += Exponential(lambda);
		}
		return ans;
	}
	else {
		throw ex_rngError();
	}
}

double RngClass::Gamma(double scale, double shape) { // Gamma(scale,shape) distribution {
	if ( scale >= 0 && shape > 0.0 ) {
		if ( scale == 0.0 ) {
			return 0.0;
		}
		else {
			return gsl_ran_gamma(gslRng, shape, scale);
		}
	}
	else {
		throw ex_rngError();
	}
}

double RngClass::Beta(double alpha, double beta) { // Beta(alpha,beta) distribution
	if ( alpha > 0.0 && beta > 0.0 ) {
		return gsl_ran_beta(gslRng, alpha, beta);
	}
	else {
		throw ex_rngError();
	}
}

double RngClass::BetaMnSd(double M, double SD)
{
	if ( M <= 0.0 || M >= 1.0 || SD <= 0.0 ) throw ex_rngError();
	double a, sumab;
	sumab = M*(1.0-M)/(SD*SD) - 1.0;
	a = M*sumab;
	return Beta(a,sumab-a);
}

int RngClass::Binomial(int n, double p) { // Binom(n,p)
	if ( p >= 0.0 && p <= 1.0 && n >= 0 ) {
		int ans = gsl_ran_binomial(gslRng, p, unsigned(n));
		if ( ans >= 0 )	return ans;
		else throw ex_rngError();
	}
	else {
		throw ex_rngError();
	}
}

int RngClass::PosBinomial(int n, double p) {
	/* gets a sample x from the binomial distribution
	 * conditioned on x > 0.
	 */

	/* handle some trivial cases */
	int x = 0;
	if ( n == 0 || p == 0 ) throw ex_rngError();
	if ( n == 1 ) return 1;

	/* handle non-trivial cases */
	double np = n*p;
	if ( np > 1 ) {
		/* use a naive method for large np */
		do {
			x = Binomial(n,p);
		} while ( x == 0 );
	}
	else {
		if ( np > 1e-6 ) {
			/* chop-down method */
			int n1 = n; double qn_aux = 1.0-p; double qn = 1.0;
			while ( n1 ) { // fast method for calculating qn := (1-p)^n
				if (n1 & 1) qn *= qn_aux;
				qn_aux *= qn_aux;  n1 >>= 1;
			}
			double pp = p / (1-p);
			double pdf = n * pp * qn; // probability of x = 1
			int ell = 1; n1 = n;
			double ran = Uniform() * (1.0-qn); // condition on x > 0
			while ( ran > pdf && ell < n ) {
				ran -= pdf;
				ell++; n1--;
				pdf *= pp * n1;
				ran *= ell;
			}
			x = ell;
		}
		else {
			/* use Poisson method
			 * First make an exp(1) jump, and scale so that it is between 0 and np. Then start
			 * counting exp(1) jumps below np.
			 */
			double u0 = Uniform(); double uell(0.0);
			double expr_np = exp(-np); // don't take logs all the time...
			double expr_j0 = 1.0-u0*(1.0-expr_np); // take products, calc exp(np) and don't calc logs!!
			if ( expr_j0 <= expr_np ) x = 1;
			else {
				int ell = 0;
				double expr_jell = expr_j0;
				while ( expr_jell > expr_np ) {
					ell++; // take another exponential jump.
					uell = Uniform();
					expr_jell *= uell;
				}
				x = ell;
			}
		}
	}
	return x;
}

int RngClass::Poisson(double lambda) { // Poisson(lambda)
	if ( lambda >= 0.0 ) {
		if ( lambda == 0.0 ) {
			return 0.0;
		}
		else {
			return gsl_ran_poisson(gslRng, lambda);
		}
	}
	else {
		throw ex_rngError();
	}
}

int RngClass::Hypergeometric(int n, int K, int N) { // draws, successes, population size
	if ( N >= 0 && K >= 0 && n >= 0 && K <= N && n <= N ) {
		unsigned ans = gsl_ran_hypergeometric(gslRng, K, N-K, n);
		if ( ans >= 0 )	return ans;
		else throw ex_rngError();
	}
	else {
		throw ex_rngError();
	}
}

void RngClass::Shuffle(int* xs, int min, int n) { // a random sample (unique)
	if ( n > 0 ) {
		for ( int i = 0; i < n; ++i ) xs[i] = min + i;
		gsl_ran_shuffle(gslRng, xs, size_t(n), sizeof(int));
	}
	else {
		throw ex_rngError();
	}
}

