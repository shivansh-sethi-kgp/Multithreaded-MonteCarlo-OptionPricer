#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>
#include <memory>
#include <chrono>
#include <future> // Required for multi-threading
#include <thread> // Required to check hardware cores
#include <vector>
#include <cctype>

using namespace std;

class Payoff
{
public:
    virtual double cal_payout(double final_stock_price, double avg_stock_price) const = 0;
    virtual ~Payoff() = default;
};

class Pay_Call : public Payoff
{
private:
    double strike;

public:
    Pay_Call(double strike)
    {
        this->strike = strike;
    }

    double cal_payout(double final_stock_price, double avg_stock_price) const override
    {
        return max(final_stock_price - strike, 0.0);
    }
};

class Pay_Put : public Payoff
{
private:
    double strike;

public:
    Pay_Put(double strike)
    {
        this->strike = strike;
    }

    double cal_payout(double final_stock_price, double avg_stock_price) const override
    {
        return max(strike - final_stock_price, 0.0);
    }
};

class Pay_Asian_Fixed_Strike_Call : public Payoff
{
private:
    double strike;

public:
    Pay_Asian_Fixed_Strike_Call(double strike)
    {
        this->strike = strike;
    }

    double cal_payout(double final_stock_price, double avg_stock_price) const override
    {
        return max(avg_stock_price - strike, 0.0);
    }
};

class Pay_Asian_Fixed_Strike_Put : public Payoff
{
private:
    double strike;

public:
    Pay_Asian_Fixed_Strike_Put(double strike)
    {
        this->strike = strike;
    }

    double cal_payout(double final_stock_price, double avg_stock_price) const override
    {
        return max(strike - avg_stock_price, 0.0);
    }
};

class Pay_Asian_Floating_Strike_Call : public Payoff
{
public:
    double cal_payout(double final_stock_price, double avg_stock_price) const override
    {
        return max(final_stock_price - avg_stock_price, 0.0);
    }
};

class Pay_Asian_Floating_Strike_Put : public Payoff
{
public:
    double cal_payout(double final_stock_price, double avg_stock_price) const override
    {
        return max(avg_stock_price - final_stock_price, 0.0);
    }
};

class Option_Identifier
{
public:
    static unique_ptr<Payoff> createOption(const string &type, double strike)
    {

        if (type == "vanilla call")
        {

            return make_unique<Pay_Call>(strike);
        }
        else if (type == "vanilla put")
        {

            return make_unique<Pay_Put>(strike);
        }
        else if (type == "asian fixed strike call")
        {

            return make_unique<Pay_Asian_Fixed_Strike_Call>(strike);
        }
        else if (type == "asian fixed strike put")
        {

            return make_unique<Pay_Asian_Fixed_Strike_Put>(strike);
        }
        else if (type == "asian floating strike call")
        {

            return make_unique<Pay_Asian_Floating_Strike_Call>();
        }
        else if (type == "asian floating strike put")
        {

            return make_unique<Pay_Asian_Floating_Strike_Put>();
        }
        else
        {

            throw invalid_argument("Invalid option type. Ensure input is lowercase and strictly matches the provided list");
        }
    }
};

vector<double> monte_carlo_each_core(int paths_each_core, const Payoff *this_option, double spot, double strike, double first, double second_1, double h, int eff_n)
{

    random_device rd;
    mt19937 gen(rd());
    normal_distribution<double> std_nor(0, 1);

    double this_core_sum_payoff_base = 0.0;
    double this_core_sum_payoff_up = 0.0;
    double this_core_sum_payoff_down = 0.0;

    for (int path = 0; path < paths_each_core; path++)
    {

        double S_0_base = spot;
        double S_t_base = 0.0;

        double S_0_up = spot + h;
        double S_t_up = 0.0;

        double S_0_down = spot - h;
        double S_t_down = 0.0;

        double sum_path_price_base = 0.0;
        double sum_path_price_up = 0.0;
        double sum_path_price_down = 0.0;

        for (int day = 0; day < eff_n; day++)
        {

            double z = std_nor(gen);

            double exp_factor = exp(first + second_1 * z);

            S_t_base = S_0_base * exp_factor;
            S_t_up = S_0_up * exp_factor;
            S_t_down = S_0_down * exp_factor;

            sum_path_price_base += S_t_base;
            sum_path_price_up += S_t_up;
            sum_path_price_down += S_t_down;

            S_0_base = S_t_base;
            S_0_up = S_t_up;
            S_0_down = S_t_down;
        }

        // S_t now represents the terminal stock price at maturity
        double avg_path_price_base = sum_path_price_base / eff_n;
        double avg_path_price_up = sum_path_price_up / eff_n;
        double avg_path_price_down = sum_path_price_down / eff_n;

        this_core_sum_payoff_base += this_option->cal_payout(S_t_base, avg_path_price_base);
        this_core_sum_payoff_up += this_option->cal_payout(S_t_up, avg_path_price_up);
        this_core_sum_payoff_down += this_option->cal_payout(S_t_down, avg_path_price_down);
    }

    return {this_core_sum_payoff_base, this_core_sum_payoff_up, this_core_sum_payoff_down};
}

int main()
{

    double spot, r, sigma, strike;
    int n;
    string type;

    // Taking user input
    cout << "Enter the current stock price\n";
    cin >> spot;
    cout << "Enter the strike price\n";
    cin >> strike;
    cout << "Enter the current risk free rate\n";
    cin >> r;
    cout << "Enter the current volatility\n";
    cin >> sigma;
    cout << "Days to Maturity\n";
    cin >> n;

    // Clear the input buffer before reading the string to avoid consuming leftover newline characters
    cin.ignore();

    cout << "Enter the option type (vanilla call, vanilla put, asian fixed strike call, asian fixed strike put, asian floating strike call, asian floating strike put)\n";
    getline(cin, type);
    transform(type.begin(), type.end(), type.begin(), ::tolower);

    // Start the Stopwatch
    auto start_time = chrono::high_resolution_clock::now();

    const int trading_days_per_year = 252;
    const int calendar_days_per_year = 365;

    double delta_t = 1.0 / trading_days_per_year;

    int eff_n = (n * trading_days_per_year) / calendar_days_per_year;
    if (eff_n == 0)
    {
        eff_n = 1;
    }

    double h = 0.01;

    double first = (r - (sigma * sigma / 2.0)) * (delta_t);
    double second_1 = sigma * sqrt(delta_t);

    unique_ptr<Payoff> this_option = Option_Identifier::createOption(type, strike);

    // Multithreaded Monte Carlo Execution

    int total_paths = 1000000;

    int num_cores = std::thread::hardware_concurrency();

    if (num_cores == 0)
    {
        num_cores = 4;
    }

    int paths_per_core = total_paths / num_cores;

    // Container to hold future results from worker threads
    vector<future<vector<double>>> core_local_sums;

    // Dispatch Monte Carlo paths across available CPU cores asynchronously
    for (int i = 0; i < num_cores; i++)
    {
        core_local_sums.push_back(async(launch::async, monte_carlo_each_core, paths_per_core, this_option.get(), spot, strike, first, second_1, h, eff_n));
    }

    double sum_payoff_base = 0.0;
    double sum_payoff_up = 0.0;
    double sum_payoff_down = 0.0;

    for (auto &f : core_local_sums)
    {

        vector<double> each_local_sum_vector = f.get();

        sum_payoff_base += each_local_sum_vector[0];
        sum_payoff_up += each_local_sum_vector[1];
        sum_payoff_down += each_local_sum_vector[2];
    }

    double avg_payoff_base = sum_payoff_base / total_paths;
    double avg_payoff_up = sum_payoff_up / total_paths;
    double avg_payoff_down = sum_payoff_down / total_paths;

    double T = (double)eff_n / trading_days_per_year;

    double option_price_base = avg_payoff_base * exp(-r * T);
    double option_price_up = avg_payoff_up * exp(-r * T);
    double option_price_down = avg_payoff_down * exp(-r * T);

    double delta = (option_price_up - option_price_down) / (2 * h);
    double gamma = (option_price_up + option_price_down - 2.0 * option_price_base) / (h * h);

    // Stop the Stopwatch
    auto end_time = chrono::high_resolution_clock::now();

    // Calculate the difference in seconds
    chrono::duration<double> elapsed_seconds = end_time - start_time;

    cout << "\nSimulated option price : $" << option_price_base << "\n";
    cout << "Delta of the Option : " << delta << "\n";
    cout << "Gamma of the Option : " << gamma << "\n";
    cout << "Computation Time : " << elapsed_seconds.count() << " seconds\n";

    return 0;
}