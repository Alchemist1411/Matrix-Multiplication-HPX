#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>

#include <iostream>
#include <chrono>
#include <vector>

using matrix = std::vector<std::vector<std::size_t>>;
using time_result = std::pair<std::chrono::duration<double>, matrix>;


size_t inner_loop(const matrix& M1, const matrix& M2, size_t row, size_t col, size_t n)
{
    std::size_t partial_result = 0;
    hpx::experimental::for_loop(hpx::execution::par, 0, n, [&](auto i) { partial_result += M1[row][i] * M2[i][col]; });

    return partial_result;
}

// For sequential execution
time_result sequential(const matrix& m1, const matrix& m2, const size_t n)
{
    matrix result(n, std::vector<size_t>(n));

    auto before = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            result[i][j] += inner_loop(m1, m2, i, j, n);
        }
    }
    auto after = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_taken = after - before;

    return std::make_pair(time_taken, result);
}

// For parallel execution
time_result parallel(const matrix& m1, const matrix& m2, const size_t n)
{
    matrix result(n, std::vector<size_t>(n));

    auto before = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < n; i++)
    {
        hpx::experimental::for_loop(hpx::execution::par, 0, n,
            [&](auto j) { result[i][j] = inner_loop(m1, m2, i, j, n); });
    }
    auto after = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_taken = after - before;

    return std::make_pair(time_taken, result);
}

int hpx_main(hpx::program_options::variables_map& vm)
{
    std::size_t n = vm["n"].as<std::size_t>();

    matrix m1(n, std::vector<size_t>(n));
    matrix m2(n, std::vector<size_t>(n));

    auto seq_res = sequential(m1, m2, n);
    auto par_res = parallel(m1, m2, n);

    std::cout << "Time required by Sequential : " << seq_res.first.count()<< "s\n";
    std::cout << "Time required by Parallel : " << par_res.first.count()<< "s\n";

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()("n", value<std::size_t>()->default_value(200), "Matrix dimension");

    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;

    return hpx::init(argc, argv, init_args);
}