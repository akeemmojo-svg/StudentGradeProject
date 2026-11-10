

#pragma once

class ContainerBenchmark
{
public:
    // Original benchmark
    static void runAllBenchmarks();

    // Strategy 1: Copy students into passed and failed containers
    static void runStrategy1();

    // Strategy 2: Move failed students out of the original container
    static void runStrategy2();

    // Optimized strategy: stable_partition and list::splice
    static void runOptimizedStrategy();
};
