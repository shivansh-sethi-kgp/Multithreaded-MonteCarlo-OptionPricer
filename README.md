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

   Clone the repository:
   
   ```bash
   git clone [https://github.com/YourUsername/YourRepositoryName.git](https://github.com/YourUsername/YourRepositoryName.git)
   cd YourRepositoryName