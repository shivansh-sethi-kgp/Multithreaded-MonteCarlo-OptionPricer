# Multithreaded Monte Carlo Options Pricer

A high-performance, object-oriented C++ options pricer utilizing Monte Carlo simulations to price exotic and vanilla contracts. The computational workload is distributed asynchronously across available CPU cores to minimize execution time.

## Mathematical Formulation

The simulation models the underlying asset's trajectory using Geometric Brownian Motion (GBM) under the risk-neutral measure:

$$S_{t+\Delta t} = S_t \exp\left( \left( r - \frac{\sigma^2}{2} \right) \Delta t + \sigma \sqrt{\Delta t} Z \right)$$

where $Z \sim \mathcal{N}(0,1)$ is a standard normal random variable. 

### Variance Reduction for Greeks

The pricer calculates Delta and Gamma using finite difference approximations. To drastically reduce variance and computation time, the up-shock ($S_0 + h$) and down-shock ($S_0 - h$) paths are generated using **Common Random Numbers (CRN)** within the same thread execution block:

$$\Delta = \frac{V(S_0 + h) - V(S_0 - h)}{2h}$$
$$\Gamma = \frac{V(S_0 + h) + V(S_0 - h) - 2V(S_0)}{h^2}$$

## Supported Option Types

Leveraging the Factory Design Pattern, the pricer dynamically allocates polymorphic payoff functions. Currently supported structures include:

* `vanilla call` & `vanilla put`
* `asian fixed strike call` & `asian fixed strike put`
* `asian floating strike call` & `asian floating strike put`

## System Requirements & Build Instructions

This project requires a C++17 compliant compiler and CMake (3.10+). It utilizes `std::async` and `<future>` for concurrency, automatically detecting system hardware threads to partition the workload.

### Compilation via CMake

1. Clone the repository:

   ```bash
   git clone [https://github.com/YourUsername/YourRepositoryName.git](https://github.com/YourUsername/YourRepositoryName.git)
   cd YourRepositoryName

2. Create a build directory and configure the project:

   mkdir build
   cd build
   cmake ..

3. Compile the executable:

   cmake --build .

4. Run the simulation:

   ./pricer


## Sample Output

Below is an example execution calculating the price and Greeks for a standard at-the-money vanilla call option using 1,000,000 Monte Carlo paths distributed across available CPU cores:

```text
Enter the current stock price
100
Enter the strike price
100
Enter the current risk free rate
0.05
Enter the current volatility
0.18
Days to Maturity
90
Enter the option type (vanilla call, vanilla put, asian fixed strike call, asian fixed strike put, asian floating strike call, asian floating strike put)
asian floating strike call

Simulated option price : $2.3385
Delta of the Option : 0.023385
Gamma of the Option : 1.77636e-11
Computation Time : 1.59206 seconds