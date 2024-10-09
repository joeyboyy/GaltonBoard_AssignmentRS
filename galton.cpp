/*! @file galton.cpp
* @brief Galton board simulations
* @author Manuel Joey Becklas
*
**/
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <string>

std::mt19937 random_number_engine(*(int*)"MANU");
std::uniform_int_distribution<int> uniform_distr(0, 1);

/* Simulate a single ball drop in a galton board of size n 
 *
 * Returns the index of the row where the ball ends up (on interval [0,...,n]).
 */
int simulate_galton(int n)
{
    int ret = 0;
    for (int i = 0; i < n; i++)
    {
        ret += uniform_distr(random_number_engine);
    }
    return ret;
}

/* Returns pdf(x) for a normal distribution with expectation mu and variance var.
 * 
 */
double normal(double x, double mu, double var)
{
    double res = exp(-(x-mu)*(x-mu)/(2*var));
    res = res / sqrt(2*M_PI*var);
    return res;
}

/* Returns pmf(k) for a binomial distribution with paramaters n in IN and p in [0,1] */
double binomial(int k, int n, double p = 0.5)
{
    double res = 1.;
    // calculate binomial coefficient avoiding huge or tiny numbers
    for (int i = 0; i < k; i++)
    {
        double i_th_term = (double)(n-i) / (double)(k-i);
        res *= i_th_term;
    }
    res *= pow(p, k) * pow(1 - p, n-k);
    return res;
}

/* Main: Run simulations and save results
 *
 */
int main()
{
    const int num_balls = 1000000; // max number of balls (checkpointing every power of 10 balls)
    std::vector<int> heights = {5,10,50,100,1000}; // heights

    for (int height : heights)
    {
        std::cerr << "Simulating Galton board of height " << height << "... ";

        // columns the num_balls may end up in
        std::vector<int> board(height+1, 0);
        
        // run simulation
        int checkpoint = 10;
        for (int i = 0; i <= num_balls; i++)
        {
            int res = simulate_galton(height);
            board[res]++;
            if (i == checkpoint)
            {
                // output simulation results
                std::ofstream outfile_sim("../out/sim_h" + std::to_string(height) + "_b" + std::to_string(i) + ".dat");
                for (int j = 0; j <= height; j++)
                {
                    outfile_sim << j << "\t" << board[j] << "\n";
                }

                // output corresponding binomial values
                constexpr double p = 0.5;
                std::ofstream outfile_binom("../out/binom_h" + std::to_string(height) + "_b" + std::to_string(i) + ".dat");
                double msqerr = 0; // mean squared error binomial pmf to normal pdf
                double simmaxdev = 0.; // maximum deviation simulation to binomial*nballs
                for (int k = 0; k <= height; k++)
                {
                    double binom = binomial(k, height, p);
                    double eps = sqrt(10*binom*(1.-binom) / i); // eps such that from weak l.l.n. P(|sumX_i - nEX_i| >= eps*n) <= 0.1
                    double simdev = abs(binom*i - board[k]);
                    simmaxdev = (simdev > simmaxdev) ? simdev : simmaxdev;
                    outfile_binom << k << "\t" << binom * i << "\t"
                                  << eps * i << "\t# deviation of simulation: " << simdev << "\n";

                    double norm = normal(k, p*height, height*p*(1-p));
                    msqerr += (binom - norm)*(binom - norm);
                }
                msqerr /= (height+1);
                outfile_binom << "\n# mean squared error as asked in assignment: " << msqerr;
                outfile_binom << "\n# maximum deviation simulation/binomial over all k: " << simmaxdev;

                // output normal distribution values
                std::ofstream outfile_normal("../out/normal_h" + std::to_string(height) + "_b" + std::to_string(i) + ".dat");
                constexpr int samples = 1000;
                for (int s = 0; s <= samples; s++)
                {
                    double x = s/(double)samples * (height + 1)  - 0.5;
                    double norm = normal(x, p*height, height*p*(1-p));
                    outfile_normal << x << "\t" << norm * i << "\n";
                }

                checkpoint *= 10;
            }
        }
        std::cerr << "Simulation successful!\n";
    }
}